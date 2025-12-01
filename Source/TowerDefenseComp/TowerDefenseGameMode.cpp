// Fill out your copyright notice in the Description page of Project Settings.


#include "TowerDefenseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "EnemyCube.h"
#include "SpawnPoint.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"

ATowerDefenseGameMode::ATowerDefenseGameMode()
{
    // optional default PawnClass etc.
}

void ATowerDefenseGameMode::BeginPlay()
{
    Super::BeginPlay();

    SpawnedCount = 0;
    ActiveEnemies.Empty();

    // If no spawn points were assigned in the editor, try to auto-find them:
    if (SpawnPoints.Num() == 0)
    {
        // 1) Try to find actors tagged "SpawnPoint"
        TArray<AActor*> FoundByTag;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("SpawnPoint"), FoundByTag);
        if (FoundByTag.Num() > 0)
        {
            SpawnPoints = FoundByTag;
        }
        else
        {
            // 2) Try to find any ATargetPoint actors (useful if you placed TargetPoints)
            TArray<AActor*> FoundTargets;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), FoundTargets);
            if (FoundTargets.Num() > 0)
            {
                SpawnPoints = FoundTargets;
            }
        }
    }

    if (HUDWidgetClass)
    {
        HUDWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
        if (HUDWidgetInstance)
        {
            HUDWidgetInstance->AddToViewport();
        }
    }
    // Only the server/GameMode should create the spawn-point actor.
    if (SpawnActorClass)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        // spawn at the numeric location specified in defaults (you can change this per-game-mode instance if desired)
        SpawnPointActor = GetWorld()->SpawnActor<AActor>(SpawnActorClass, SpawnLocation, SpawnRotation, Params);

        UE_LOG(LogTemp, Log, TEXT("SpawnPointActor spawned: %s"), SpawnPointActor ? *SpawnPointActor->GetName() : TEXT("null"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnActorClass not set in GameMode defaults"));
    }
}

void ATowerDefenseGameMode::SpawnEnemy(int32 Amount)
{
    ToSpawnRemaining = Amount;
    bIsSpawning = true;
    UpdateSpawnState();

    // Start timer to spawn repeatedly
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ATowerDefenseGameMode::SpawnTick, SpawnInterval, true, 0.0f);
}

void ATowerDefenseGameMode::SpawnTick()
{
    if (!EnemyClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnEnemy: EnemyClass null"));
        return;
    }

    FVector Loc = SpawnLocation;
    FRotator Rot = SpawnRotation;

    if (SpawnPointActor)
    {
        Loc = SpawnPointActor->GetActorLocation();
        Rot = SpawnPointActor->GetActorRotation();
    }

    /*FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    APawn* Spawned = GetWorld()->SpawnActor<APawn>(EnemyClass, Loc, Rot, Params);*/

    if (ToSpawnRemaining <= 0)
    {
        // No more to spawn this run: stop timer, keep bIsSpawning true until alive enemies reach 0
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
        return;
    }

    /*FTransform SpawnTransform = GetWorld()->GetFirstPlayerController()->GetPawn()
        ? GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorTransform()
        : FTransform::Identity;*/
    // You probably want a proper spawn location instead of this example transform.

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APawn* Spawned = GetWorld()->SpawnActor<APawn>(EnemyClass, Loc, Rot, SpawnParams);
    if (Spawned)
    {
        ToSpawnRemaining--;
        AliveEnemies++;

        // Track when it dies; OnDestroyed works for actors that get Destroy() called on death
        //Spawned->OnDestroyed.AddDynamic(this, &ATowerDefenseGameMode::HandleEnemyDestroyed);
    }
    else
    {
        // Could not spawn; avoid infinite loop by decrementing. Change if you want retry logic.
        ToSpawnRemaining--;
    }


    if (Spawned)
    {
        UE_LOG(LogTemp, Log, TEXT("Spawned Enemy at %s"), *Loc.ToString());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Spawned Enemy at %s"), *Loc.ToString()));
        }
    }
}

void ATowerDefenseGameMode::HandleEnemyDestroyed(AActor* DestroyedActor)
{
    AliveEnemies = FMath::Max(0, AliveEnemies - 1);

    // If we've finished spawning (timer not active) and there are no more alive enemies, end the run
    if (AliveEnemies == 0 && !GetWorld()->GetTimerManager().IsTimerActive(SpawnTimerHandle) && bIsSpawning)
    {
        bIsSpawning = false;
        UpdateSpawnState();
    }

    if (!DestroyedActor) return;
    AEnemyCube* Enemy = Cast<AEnemyCube>(DestroyedActor);
    if (!Enemy) return;

    UE_LOG(LogTemp, Log, TEXT("HandleEnemyDestroyed called for %s"), *Enemy->GetName());

    ActiveEnemies.Remove(Enemy);

    // Also try to unbind the other dynamic delegates (safe even if already removed)
    Enemy->OnEnemyKilled.RemoveDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);
    Enemy->OnEnemyReachedGoal.RemoveDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);

    CheckWaveComplete();
}

void ATowerDefenseGameMode::UpdateSpawnState()
{
    // Allow spawn only when not currently in a spawn-run and no alive enemies exist
    const bool bCanSpawn = (!bIsSpawning && AliveEnemies == 0);
    OnSpawnStateChanged.Broadcast(bCanSpawn);
}

void ATowerDefenseGameMode::StartWave()
{
    if (!EnemyClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("TowerDefenseGameMode::StartWave - EnemyClass not set"));
        return;
    }

    // Reset
    SpawnedCount = 0;
    ActiveEnemies.Empty();
    GetWorldTimerManager().ClearTimer(SpawnTimerHandle);

    // Spawn first immediately (you can change behavior if you want delay)
    SpawnNextEnemy();

    // If more to spawn, start periodic timer
    if (EnemiesPerWave > 1)
    {
        GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ATowerDefenseGameMode::SpawnNextEnemy, SpawnInterval, true);
    }
}

void ATowerDefenseGameMode::SpawnNextEnemy()
{
    if (SpawnedCount >= EnemiesPerWave)
    {
        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
        return;
    }

    // Choose spawn transform:
    FTransform SpawnTransform = FTransform::Identity;

    if (SpawnPoints.Num() > 0 && SpawnPoints.IsValidIndex(SpawnedCount % SpawnPoints.Num()) && SpawnPoints[SpawnedCount % SpawnPoints.Num()])
    {
        SpawnTransform = SpawnPoints[SpawnedCount % SpawnPoints.Num()]->GetActorTransform();
    }
    else
    {
        // Fallback: use player pawn transform if available
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (PlayerPawn)
        {
            SpawnTransform = PlayerPawn->GetActorTransform();
        }
        else
        {
            // Final fallback: keep identity (0,0,0)
            SpawnTransform = FTransform::Identity;
        }

        /*if (NewEnemy)
        {
            ActiveEnemies.AddUnique(NewEnemy);
            NewEnemy->OnEnemyKilled.AddUniqueDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);
            NewEnemy->OnEnemyReachedGoal.AddUniqueDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);

            // Optional: also bind to OnDestroyed as backup
            NewEnemy->OnDestroyed.AddDynamic(this, &ATowerDefenseGameMode::HandleEnemyDestroyed);

            UE_LOG(LogTemp, Log, TEXT("Spawned %s and bound delegates. Active count: %d"), *NewEnemy->GetName(), ActiveEnemies.Num());
        }*/
    }

    AEnemyCube* NewEnemy = SpawnEnemyAtTransform(SpawnTransform);
    if (NewEnemy)
    {
        ActiveEnemies.Add(NewEnemy);
        NewEnemy->OnEnemyKilled.AddDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);
        NewEnemy->OnEnemyReachedGoal.AddDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);
    }

    SpawnedCount++;

    if (SpawnedCount >= EnemiesPerWave)
    {
        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
        CheckWaveComplete();
    }
}

AEnemyCube* ATowerDefenseGameMode::SpawnEnemyAtTransform(const FTransform& Transform)
{
    if (!EnemyClass) return nullptr;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    UWorld* World = GetWorld();
    if (!World) return nullptr;

    AEnemyCube* Spawned = World->SpawnActorDeferred<AEnemyCube>(EnemyClass, Transform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
    if (Spawned)
    {
        // If you need to initialize the enemy (set navpath, health, etc) do it here before finishing spawn
        UGameplayStatics::FinishSpawningActor(Spawned, Transform);
    }
    return Spawned;
}

void ATowerDefenseGameMode::HandleEnemyRemoved(AEnemyCube* Enemy)
{
    if (!Enemy) return;

    // Remove from active list
    ActiveEnemies.Remove(Enemy);

    // Unbind to be tidy (not strictly necessary if enemy is destroyed)
    Enemy->OnEnemyKilled.RemoveDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);
    Enemy->OnEnemyReachedGoal.RemoveDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);

    CheckWaveComplete();
}

void ATowerDefenseGameMode::CheckWaveComplete()
{
    // Wave is complete when we've spawned all enemies and there are no active enemies alive
    if (SpawnedCount >= EnemiesPerWave && ActiveEnemies.Num() == 0)
    {
        OnWaveFinished.Broadcast();
    }
}
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
    // defaults if needed
}

void ATowerDefenseGameMode::BeginPlay()
{
    Super::BeginPlay();

    SpawnedCount = 0;
    ActiveEnemies.Empty();
    bWaveActive = false;
    UpdateSpawnState();

    // Auto-find spawn points if none assigned in editor
    if (SpawnPoints.Num() == 0)
    {
        TArray<AActor*> FoundByTag;
        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("SpawnPoint"), FoundByTag);
        if (FoundByTag.Num() > 0)
        {
            SpawnPoints = FoundByTag;
        }
        else
        {
            TArray<AActor*> FoundTargets;
            UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), FoundTargets);
            if (FoundTargets.Num() > 0)
            {
                SpawnPoints = FoundTargets;
            }
        }
    }

    // Optional HUD
    if (HUDWidgetClass)
    {
        HUDWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
        if (HUDWidgetInstance)
        {
            HUDWidgetInstance->AddToViewport();
        }
    }

    // Optional single spawn-point actor
    if (SpawnActorClass)
    {
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnPointActor = GetWorld()->SpawnActor<AActor>(SpawnActorClass, SpawnLocation, SpawnRotation, Params);
        UE_LOG(LogTemp, Log, TEXT("SpawnPointActor spawned: %s"), SpawnPointActor ? *SpawnPointActor->GetName() : TEXT("null"));
    }
    else
    {
        UE_LOG(LogTemp, Verbose, TEXT("SpawnActorClass not set in GameMode defaults"));
    }
}

void ATowerDefenseGameMode::StartWave()
{
    if (!EnemyClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("StartWave: EnemyClass not set"));
        return;
    }

    if (bWaveActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("StartWave called while wave already active"));
        return;
    }

    // Reset counters and state
    SpawnedCount = 0;
    ActiveEnemies.Empty();

    bWaveActive = true;
    UpdateSpawnState();

    // Start spawning repeatedly (first spawn immediately)
    SpawnNextEnemy();
    if (EnemiesPerWave > 1 && SpawnInterval > 0.0f)
    {
        GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ATowerDefenseGameMode::SpawnNextEnemy, SpawnInterval, true);
    }
}

void ATowerDefenseGameMode::SpawnNextEnemy()
{
    if (!EnemyClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnNextEnemy: EnemyClass null"));
        return;
    }

    if (SpawnedCount >= EnemiesPerWave)
    {
        // Stop timer if active
        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
        // wave spawn completed; we'll finish when active enemies reach 0 (CheckWaveComplete will run when enemies removed)
        return;
    }

    // Choose spawn transform
    FTransform SpawnTransform = FTransform::Identity;
    if (SpawnPoints.Num() > 0 && SpawnPoints.IsValidIndex(SpawnedCount % SpawnPoints.Num()) && SpawnPoints[SpawnedCount % SpawnPoints.Num()])
    {
        SpawnTransform = SpawnPoints[SpawnedCount % SpawnPoints.Num()]->GetActorTransform();
    }
    else if (SpawnPointActor)
    {
        SpawnTransform = SpawnPointActor->GetActorTransform();
    }
    else
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (PlayerPawn) SpawnTransform = PlayerPawn->GetActorTransform();
        // otherwise leave identity
    }

    AEnemyCube* NewEnemy = SpawnEnemyAtTransform(SpawnTransform);
    if (NewEnemy)
    {
        // Track via weak ptr (safe if actor gets destroyed)
        ActiveEnemies.AddUnique(TWeakObjectPtr<AEnemyCube>(NewEnemy));

        NewEnemy->CurrentHealth1 = NewEnemy->MaxHealth1 * HealthMultiplier;

        // Bind both game-specific and universal destroyed handlers
        NewEnemy->OnEnemyKilled.AddDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);
        NewEnemy->OnEnemyReachedGoal.AddDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);
        NewEnemy->OnDestroyed.AddDynamic(this, &ATowerDefenseGameMode::HandleEnemyDestroyed);

        UE_LOG(LogTemp, Log, TEXT("Spawned Enemy: %s (active: %d)"), *NewEnemy->GetName(), ActiveEnemies.Num());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn enemy"));
    }

    SpawnedCount++;

    if (SpawnedCount >= EnemiesPerWave)
    {
        // We've started spawning the last enemy — clear timer so no more spawns are done.
        GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
        // If no active enemies remain, finish now:
        CheckWaveComplete();
    }
}

AEnemyCube* ATowerDefenseGameMode::SpawnEnemyAtTransform(const FTransform& Transform)
{
    if (!EnemyClass) return nullptr;

    UWorld* World = GetWorld();
    if (!World) return nullptr;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // Use FinishSpawning if you want deferred init; simplified direct spawn below:
    AEnemyCube* Spawned = World->SpawnActor<AEnemyCube>(EnemyClass, Transform, Params);
    return Spawned;
}

void ATowerDefenseGameMode::HandleEnemyRemoved(AEnemyCube* Enemy)
{
    if (!Enemy) return;

    // Remove any weak ptrs that point to this enemy
    ActiveEnemies.RemoveAll([Enemy](const TWeakObjectPtr<AEnemyCube>& Ptr)
        {
            return Ptr.IsValid() && Ptr.Get() == Enemy;
        });

    // Unbind to be tidy (safe even if already removed)
    Enemy->OnEnemyKilled.RemoveDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);
    Enemy->OnEnemyReachedGoal.RemoveDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);
    Enemy->OnDestroyed.RemoveDynamic(this, &ATowerDefenseGameMode::HandleEnemyDestroyed);

    UE_LOG(LogTemp, Verbose, TEXT("HandleEnemyRemoved: %s (remaining active: %d)"), *Enemy->GetName(), ActiveEnemies.Num());

    CheckWaveComplete();
}

void ATowerDefenseGameMode::HandleEnemyDestroyed(AActor* DestroyedActor)
{
    if (!DestroyedActor) return;

    AEnemyCube* Enemy = Cast<AEnemyCube>(DestroyedActor);
    if (!Enemy) return;

    // Remove this enemy's weak ptr
    ActiveEnemies.RemoveAll([Enemy](const TWeakObjectPtr<AEnemyCube>& Ptr)
        {
            return !Ptr.IsValid() || Ptr.Get() == Enemy;
        });

    // We can't meaningfully RemoveDynamic on a destroyed actor safely in all cases, but try to be tidy:
    Enemy->OnEnemyKilled.RemoveDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);
    Enemy->OnEnemyReachedGoal.RemoveDynamic(this, &ATowerDefenseGameMode::HandleEnemyRemoved);
    Enemy->OnDestroyed.RemoveDynamic(this, &ATowerDefenseGameMode::HandleEnemyDestroyed);

    UE_LOG(LogTemp, Verbose, TEXT("HandleEnemyDestroyed for %s (remaining active: %d)"), *Enemy->GetName(), ActiveEnemies.Num());

    CheckWaveComplete();
}

void ATowerDefenseGameMode::CheckWaveComplete()
{
    // Prune any invalid weak ptrs first
    ActiveEnemies.RemoveAll([](const TWeakObjectPtr<AEnemyCube>& Ptr)
        {
            return !Ptr.IsValid();
        });

    // Wave considered finished when we've spawned all enemies and there are no active enemies
    const bool bAllSpawned = (SpawnedCount >= EnemiesPerWave);
    const bool bNoActive = (ActiveEnemies.Num() == 0);

    if (bWaveActive && bAllSpawned && bNoActive)
    {
        bWaveActive = false; // ensure we only broadcast once

        HealthMultiplier = HealthMultiplier * 2; // Making each wave increase in difficulty

        UpdateSpawnState();
        UE_LOG(LogTemp, Log, TEXT("Wave finished. Spawned: %d, Active: %d"), SpawnedCount, ActiveEnemies.Num());
        OnWaveFinished.Broadcast();
    }
}

void ATowerDefenseGameMode::UpdateSpawnState()
{
    // Allow spawn only when not currently in a wave
    const bool bCanSpawn = !bWaveActive;
    OnSpawnStateChanged.Broadcast(bCanSpawn);
}

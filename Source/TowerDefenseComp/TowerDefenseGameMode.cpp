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
        Spawned->OnDestroyed.AddDynamic(this, &ATowerDefenseGameMode::HandleEnemyDestroyed);
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
}

void ATowerDefenseGameMode::UpdateSpawnState()
{
    // Allow spawn only when not currently in a spawn-run and no alive enemies exist
    const bool bCanSpawn = (!bIsSpawning && AliveEnemies == 0);
    OnSpawnStateChanged.Broadcast(bCanSpawn);
}
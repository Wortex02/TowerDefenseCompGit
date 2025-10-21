// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"
#include "PooledEnemyBase.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

AEnemySpawner::AEnemySpawner()
{
    PrimaryActorTick.bCanEverTick = false;
    CurrentWaveIndex = 0;
    ActivationIndexInWave = 0;
    AliveInWave = 0;
}

void AEnemySpawner::BeginPlay()
{
    Super::BeginPlay();
    PreSpawnAllWaves();
}

void AEnemySpawner::PreSpawnAllWaves()
{
    UWorld* World = GetWorld();
    if (!World) return;

    WavePools.Empty();
    WavePools.SetNum(Waves.Num());

    for (int32 WaveIdx = 0; WaveIdx < Waves.Num(); ++WaveIdx)
    {
        const FWaveInfo& Wave = Waves[WaveIdx];
        TArray<APooledEnemyBase*>& Pool = WavePools[WaveIdx];

        if (!Wave.EnemyClass) continue;

        for (int32 i = 0; i < Wave.Count; ++i)
        {
            // Decide spawn transform using spawn points if provided, otherwise use spawner transform
            FTransform SpawnTransform = GetActorTransform();
            if (Wave.SpawnPoints.Num() > 0)
            {
                int32 SpawnPointIndex = i % Wave.SpawnPoints.Num();
                if (AActor* SP = Wave.SpawnPoints[SpawnPointIndex])
                {
                    SpawnTransform = SP->GetActorTransform();
                }
            }

            FActorSpawnParameters Params;
            Params.Owner = this;
            // ensure actor always spawns even if collision overlaps
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            APooledEnemyBase* NewEnemy = World->SpawnActor<APooledEnemyBase>(Wave.EnemyClass, SpawnTransform, Params);
            if (NewEnemy)
            {
                // Register with spawner and prepare for pool
                NewEnemy->SetOwningSpawner(this);
                NewEnemy->PrepareForPool();
                Pool.Add(NewEnemy);
            }
        }
    }
}

bool AEnemySpawner::StartNextWave()
{
    // If there are no more waves return false
    if (CurrentWaveIndex >= Waves.Num())
    {
        return false;
    }

    // If there are currently active enemies in the wave, do not allow starting another wave until they die.
    if (AliveInWave > 0 || GetWorldTimerManager().IsTimerActive(ActivationTimerHandle))
    {
        // optionally return false so UI can show "cannot start yet"
        return false;
    }

    // Prepare to activate the next wave
    ActivationIndexInWave = 0;
    AliveInWave = 0;

    const FWaveInfo& Wave = Waves[CurrentWaveIndex];

    // If interval == 0, activate all instantly
    if (Wave.ActivationInterval <= 0.0001f)
    {
        TArray<APooledEnemyBase*>& Pool = WavePools[CurrentWaveIndex];
        for (APooledEnemyBase* E : Pool)
        {
            if (E)
            {
                E->ActivateFromPool();
                AliveInWave++;
            }
        }
        // Wave started — increment index (we'll wait for NotifyPooledEnemyDead to advance when enemies die)
        CurrentWaveIndex++;
    }
    else
    {
        // Start a repeating timer that activates next enemy at ActivationInterval
        GetWorldTimerManager().SetTimer(ActivationTimerHandle, this, &AEnemySpawner::ActivateNextPooledEnemy, Wave.ActivationInterval, true);
    }

    return true;
}

void AEnemySpawner::ActivateNextPooledEnemy()
{
    // safety
    if (CurrentWaveIndex >= WavePools.Num())
    {
        GetWorldTimerManager().ClearTimer(ActivationTimerHandle);
        return;
    }

    TArray<APooledEnemyBase*>& Pool = WavePools[CurrentWaveIndex];
    if (ActivationIndexInWave >= Pool.Num())
    {
        // finished activating all in this wave - stop the timer and advance to "waiting for deaths" state
        GetWorldTimerManager().ClearTimer(ActivationTimerHandle);

        // Advance wave index so subsequent StartNextWave works for the subsequent wave
        CurrentWaveIndex++;
        return;
    }

    APooledEnemyBase* EnemyToActivate = Pool[ActivationIndexInWave];
    if (EnemyToActivate)
    {
        EnemyToActivate->ActivateFromPool();
        AliveInWave++;
    }

    ActivationIndexInWave++;
}

void AEnemySpawner::NotifyPooledEnemyDead(APooledEnemyBase* Enemy)
{
    if (!Enemy) return;

    // Immediately prepare it for pool again (hide + disable)
    Enemy->PrepareForPool();

    // decrement alive counter safely
    AliveInWave = FMath::Max(0, AliveInWave - 1);

    // If there are no active enemies in the current wave and we are not currently activating more
    // then it's safe to consider the wave fully cleared.
    // Note: CurrentWaveIndex points to the *next* wave to start because we incremented when activation finished.
    // So we simply allow UI to press StartNextWave again when AliveInWave == 0 and no activation timer active.

    // (If you want automatic start of next wave, you can call StartNextWave() here with a small delay)
}

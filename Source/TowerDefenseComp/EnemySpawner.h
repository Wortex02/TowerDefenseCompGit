// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class APooledEnemyBase;

USTRUCT(BlueprintType)
struct FWaveInfo
{
    GENERATED_BODY()

    // Enemy class to use for this wave
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    TSubclassOf<APooledEnemyBase> EnemyClass;

    // How many enemies in the wave
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    int32 Count = 5;

    // Interval (seconds) between each activation when the wave is triggered.
    // If 0, activates all instantly.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    float ActivationInterval = 0.25f;

    // Optional spawn point actors (if empty spawner actor's transform used)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
    TArray<AActor*> SpawnPoints;
};

UCLASS()
class TOWERDEFENCECOMP_API AEnemySpawner : public AActor
{
    GENERATED_BODY()

public:
    AEnemySpawner();

protected:
    virtual void BeginPlay() override;

    /** Config: the waves (set in editor) */
    UPROPERTY(EditAnywhere, Category = "Spawner")
    TArray<FWaveInfo> Waves;

    // Pools: one TArray per wave
    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    TArray<TArray<APooledEnemyBase*>> WavePools;

    // State
    int32 CurrentWaveIndex;
    int32 ActivationIndexInWave; // next index to activate within current wave
    int32 AliveInWave; // active enemy count in current wave

    FTimerHandle ActivationTimerHandle;

    // Pre-spawn all enemies for each wave (called during BeginPlay)
    void PreSpawnAllWaves();

    // Activation helper called repeatedly (or once if interval == 0) to activate next enemy in current wave
    void ActivateNextPooledEnemy();

public:
    // Called by UI to start the next wave. BlueprintCallable so UMG can call it.
    UFUNCTION(BlueprintCallable, Category = "Spawner")
    bool StartNextWave();

    // Called by pooled enemies when they "die" to return to pool
    UFUNCTION()
    void NotifyPooledEnemyDead(APooledEnemyBase* Enemy);

    // Optional: helper to check if more waves exist
    UFUNCTION(BlueprintCallable, Category = "Spawner")
    bool HasMoreWaves() const { return CurrentWaveIndex < Waves.Num(); }
};


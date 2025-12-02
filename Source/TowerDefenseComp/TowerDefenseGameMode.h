// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TowerDefenseGameMode.generated.h"

class AEnemyCube;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnStateChanged, bool, bCanSpawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaveFinishedSignature);

UCLASS()
class TOWERDEFENSECOMP_API ATowerDefenseGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ATowerDefenseGameMode();

    // Call this from UI to start a wave
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void StartWave();

    // Broadcast when wave fully complete (all spawned and all active enemies gone)
    UPROPERTY(BlueprintAssignable, Category = "Wave")
    FOnWaveFinishedSignature OnWaveFinished;

    // Broadcasts to UI: bCanSpawn == false -> hide/disable button
    UPROPERTY(BlueprintAssignable, Category = "Spawning")
    FOnSpawnStateChanged OnSpawnStateChanged;

    // Enemy class to spawn
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    TSubclassOf<AEnemyCube> EnemyClass;

    // How many enemies to spawn during the wave
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    int32 EnemiesPerWave = 10;

    // Time between spawns
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    float SpawnInterval = 0.5f;

    // Optional spawn points to fill in the level
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    TArray<AActor*> SpawnPoints;

    float HealthMultiplier = 1.f;

protected:
    virtual void BeginPlay() override;

    // HUD widget class (optional)
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> HUDWidgetClass;

private:
    // Timer & counters
    FTimerHandle SpawnTimerHandle;
    int32 SpawnedCount = 0;

    // Active enemies currently alive (or not yet reached goal)
    UPROPERTY()
    TArray<TWeakObjectPtr<AEnemyCube>> ActiveEnemies;

    // Whether a wave is currently active (spawn run underway or active enemies present)
    bool bWaveActive = false;

    // Spawn a single enemy (called by timer)
    void SpawnNextEnemy();

    // Helper to actually spawn at a transform
    AEnemyCube* SpawnEnemyAtTransform(const FTransform& Transform);

    // Handlers bound to enemy events
    UFUNCTION()
    void HandleEnemyRemoved(AEnemyCube* Enemy);

    UFUNCTION()
    void HandleEnemyDestroyed(AActor* DestroyedActor);

    // Check whether the wave is finished and broadcast once
    void CheckWaveComplete();

    // Update UI spawn-button state
    void UpdateSpawnState();

    // Optional: spawn actor class and spawn point actor (kept from your original, still supported)
    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<AActor> SpawnActorClass;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    FVector SpawnLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    FRotator SpawnRotation = FRotator::ZeroRotator;

    UPROPERTY()
    AActor* SpawnPointActor = nullptr;

    UPROPERTY()
    UUserWidget* HUDWidgetInstance = nullptr;
};

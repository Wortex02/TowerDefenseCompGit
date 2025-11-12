// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TowerDefenseGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpawnStateChanged, bool, bCanSpawn);

UCLASS()
class TOWERDEFENSECOMP_API ATowerDefenseGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ATowerDefenseGameMode();

    virtual void BeginPlay() override;

    // The actor class that will be spawned and used as the spawn point(assign this in the GameMode defaults)
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<AActor> SpawnActorClass;

    // If you want to control the location/rotation numerically in defaults
    UPROPERTY(EditAnywhere, Category = "Spawning")
    FVector SpawnLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category = "Spawning")
    FRotator SpawnRotation = FRotator::ZeroRotator;

    // Pointer to the spawned spawn-point actor (null until spawned)
    UPROPERTY()
    AActor* SpawnPointActor = nullptr;

    // Enemy class you spawn later
    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<APawn> EnemyClass;

    // Seconds between spawns
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    float SpawnInterval = 0.5f;

    // Broadcasts to UI: bCanSpawn == false -> hide/disable button
    UPROPERTY(BlueprintAssignable, Category = "Spawning")
    FOnSpawnStateChanged OnSpawnStateChanged;

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void SpawnEnemy(int32 Amount);


    /** Widget class to create at BeginPlay (set in editor) */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> HUDWidgetClass;

private:
    FTimerHandle SpawnTimerHandle;
    int32 ToSpawnRemaining = 0;
    int32 AliveEnemies = 0;
    bool bIsSpawning = false;

    void SpawnTick();
    UFUNCTION()
    void HandleEnemyDestroyed(AActor* DestroyedActor);
    void UpdateSpawnState();

protected:
    UPROPERTY()
    UUserWidget* HUDWidgetInstance;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TowerDefenseGameMode.generated.h"

UCLASS()
class TOWERDEFENSECOMP_API ATowerDefenseGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ATowerDefenseGameMode();

    virtual void BeginPlay() override;

    /** Spawns an enemy at the SpawnPoint or origin if none set */
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void SpawnEnemy();

    /** Set this in the editor to the Enemy pawn (AEnemyCube or a Blueprint subclass) */
    UPROPERTY(EditDefaultsOnly, Category = "Spawning")
    TSubclassOf<APawn> EnemyClass;

    /** Optional: place an Actor in the level and assign it here to control where enemies spawn */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    AActor* SpawnPoint;

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void SetSpawnPoint(AActor* NewSpawnPoint);


    /** Widget class to create at BeginPlay (set in editor) */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> HUDWidgetClass;

protected:
    UPROPERTY()
    UUserWidget* HUDWidgetInstance;
};

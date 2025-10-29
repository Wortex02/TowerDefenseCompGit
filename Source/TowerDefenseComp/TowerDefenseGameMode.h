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

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void SpawnEnemy();


    /** Widget class to create at BeginPlay (set in editor) */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> HUDWidgetClass;

protected:
    UPROPERTY()
    UUserWidget* HUDWidgetInstance;
};

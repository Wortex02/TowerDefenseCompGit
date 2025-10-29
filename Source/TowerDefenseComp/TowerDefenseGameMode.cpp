// Fill out your copyright notice in the Description page of Project Settings.


#include "TowerDefenseGameMode.h"
#include "Kismet/GameplayStatics.h"
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
}

void ATowerDefenseGameMode::SpawnEnemy()
{
    if (!EnemyClass) return;

    FVector SpawnLoc = FVector::ZeroVector;
    FRotator SpawnRot = FRotator::ZeroRotator;

    if (SpawnPoint)
    {
        SpawnLoc = SpawnPoint->GetActorLocation();
        SpawnRot = SpawnPoint->GetActorRotation();
    }

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    GetWorld()->SpawnActor<APawn>(EnemyClass, SpawnLoc, SpawnRot, Params);
}

// TowerDefenseGameMode.cpp
void ATowerDefenseGameMode::SetSpawnPoint(AActor* NewSpawnPoint)
{
    SpawnPoint = NewSpawnPoint;
    UE_LOG(LogTemp, Log, TEXT("SetSpawnPoint called: %s"), SpawnPoint ? *SpawnPoint->GetName() : TEXT("null"));
}


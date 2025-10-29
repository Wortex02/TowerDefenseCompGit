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

void ATowerDefenseGameMode::SpawnEnemy()
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

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    APawn* Spawned = GetWorld()->SpawnActor<APawn>(EnemyClass, Loc, Rot, Params);

    if (Spawned)
    {
        UE_LOG(LogTemp, Log, TEXT("Spawned Enemy at %s"), *Loc.ToString());
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("Spawned Enemy at %s"), *Loc.ToString()));
        }
    }
}
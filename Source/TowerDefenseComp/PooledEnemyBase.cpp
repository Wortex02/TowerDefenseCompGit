// Fill out your copyright notice in the Description page of Project Settings.


// PooledEnemyBase.cpp
#include "PooledEnemyBase.h"
#include "EnemySpawner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"

APooledEnemyBase::APooledEnemyBase()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsActive = false;
}

void APooledEnemyBase::BeginPlay()
{
    Super::BeginPlay();
    // Spawner will immediately call PrepareForPool after spawn.
}

void APooledEnemyBase::PrepareForPool()
{
    // Safety: stop AI brain if present
    if (AAIController* AI = Cast<AAIController>(GetController()))
    {
        if (UBrainComponent* Brain = AI->GetBrainComponent())
        {
            Brain->StopLogic("Pooled");
        }
    }

    // Stop movement
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->StopMovementImmediately();
        MoveComp->DisableMovement();
    }

    // Hide and disable collision + tick
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);

    bIsActive = false;
}

void APooledEnemyBase::ActivateFromPool(const FTransform& SpawnTransform)
{
    // Teleport to spawn transform
    SetActorTransform(SpawnTransform);

    // Unhide and enable collision/tick
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);

    // Re-enable movement
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->SetMovementMode(MOVE_Walking);
    }

    // Ensure there is a controller (SpawnDefaultController will create one for pawns/characters if configured)
    if (!GetController())
    {
        SpawnDefaultController();
    }

    // Start AI brain if present
    if (AAIController* AI = Cast<AAIController>(GetController()))
    {
        if (UBrainComponent* Brain = AI->GetBrainComponent())
        {
            Brain->StartLogic();
        }
    }

    bIsActive = true;
}

void APooledEnemyBase::ReturnToPool()
{
    // Notify spawner (spawner will call PrepareForPool and update counts)
    if (OwningSpawner)
    {
        OwningSpawner->NotifyPooledEnemyDead(this);
    }
    else
    {
        // fallback: destroy if no spawner (shouldn't happen)
        Destroy();
    }
}

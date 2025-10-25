// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPoolManager.h"
#include "EnemyCube.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

AEnemyPoolManager::AEnemyPoolManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEnemyPoolManager::BeginPlay()
{
    Super::BeginPlay();
    PreFillPool();
}

void AEnemyPoolManager::PreFillPool()
{
    if (!EnemyClass) return;
    UWorld* W = GetWorld();
    if (!W) return;

    Pool.Reserve(InitialPoolSize);
    for (int32 i = 0; i < InitialPoolSize; ++i)
    {
        AEnemyCube* New = SpawnAndPrepare();
        if (New) Pool.Add(New);
    }
}

AEnemyCube* AEnemyPoolManager::SpawnAndPrepare()
{
    if (!EnemyClass) return nullptr;
    UWorld* W = GetWorld();
    if (!W) return nullptr;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AEnemyCube* New = W->SpawnActor<AEnemyCube>(EnemyClass, PoolLocation, FRotator::ZeroRotator, Params);
    if (!New) return nullptr;

    // Immediately deactivate (hide / disable collision / stop ticking / stop AI)
    New->SetActorHiddenInGame(true);
    New->SetActorEnableCollision(false);
    New->SetActorTickEnabled(false);

    // Give the enemy a reference back to the pool manager (optional convenience)
    //New->OwningPool = this;

    return New;
}

AEnemyCube* AEnemyPoolManager::GetPooledEnemy()
{
    // Find first inactive (hidden) actor
    for (AEnemyCube* E : Pool)
    {
        if (E && E->IsHidden())
        {
            E->SetActorHiddenInGame(false);
            E->SetActorEnableCollision(true);
            E->SetActorTickEnabled(true);
            //E->ResetForReuse();
            return E;
        }
    }

    // None available: maybe grow
    if (bAllowGrow)
    {
        AEnemyCube* New = SpawnAndPrepare();
        if (New)
        {
            Pool.Add(New);
            New->SetActorHiddenInGame(false);
            New->SetActorEnableCollision(true);
            New->SetActorTickEnabled(true);
            //New->ResetForReuse();
            return New;
        }
    }

    return nullptr; // pool exhausted
}

void AEnemyPoolManager::ReturnToPool(AEnemyCube* Enemy)
{
    if (!Enemy) return;

    // Stop behaviour, hide and disable collisions/tick
    //Enemy->CleanupBeforePool();

    // Move enemy to the pool location to avoid stray collisions/visibility
    Enemy->SetActorLocation(PoolLocation);
    Enemy->SetActorHiddenInGame(true);
    Enemy->SetActorEnableCollision(false);
    Enemy->SetActorTickEnabled(false);

    // Optionally reset more state (health, status effects) is handled by enemy CleanupBeforePool()
}



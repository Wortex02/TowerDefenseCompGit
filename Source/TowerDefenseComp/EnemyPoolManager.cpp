// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPoolManager.h"
/*#include "EnemyCube.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

AEnemyEnemyPoolManager::AEnemyEnemyPoolManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEnemyEnemyPoolManager::BeginPlay()
{
    Super::BeginPlay();
    PreFillPool();
}

void AEnemyEnemyPoolManager::PreFillPool()
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

AEnemyCube* AEnemyEnemyPoolManager::SpawnAndPrepare()
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

AEnemyCube* AEnemyEnemyPoolManager::GetPooledEnemy()
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

void AEnemyEnemyPoolManager::ReturnToPool(AEnemyCube* Enemy)
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
}*/


//#include "EnemyPoolManager.h"
#include "EnemyCube.h"
#include "Engine/TargetPoint.h"
#include "Engine/World.h"

AEnemyPoolManager::AEnemyPoolManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AEnemyPoolManager::BeginPlay()
{
    Super::BeginPlay();

    if (!EnemyClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyPoolManager: EnemyClass not set"));
        return;
    }

    // Ensure markers exist (optional)
    if (!PoolMarker || !SummonMarker)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyPoolManager: PoolMarker or SummonMarker not set"));
    }

    // Pre-spawn pool
    for (int32 i = 0; i < PoolSize; ++i)
    {
        FVector SpawnLocation = PoolMarker ? PoolMarker->GetActorLocation() : GetActorLocation();
        FRotator SpawnRotation = FRotator::ZeroRotator;
        FActorSpawnParameters Params;
        Params.Owner = this;
        AEnemyCube* NewEnemy = GetWorld()->SpawnActor<AEnemyCube>(EnemyClass, SpawnLocation, SpawnRotation, Params);
        if (NewEnemy)
        {
            NewEnemy->OwningPoolManager = this;
            NewEnemy->DeactivateCube();
            Pool.Add(NewEnemy);
        }
    }
}

AEnemyCube* AEnemyPoolManager::GetInactiveEnemy()
{
    for (AEnemyCube* E : Pool)
    {
        if (E && !E->IsActorTickEnabled()) // or check some bIsActive flag
        {
            return E;
        }
    }
    return nullptr;
}

AEnemyCube* AEnemyPoolManager::SummonFromPool()
{
    AEnemyCube* Enemy = GetInactiveEnemy();
    if (!Enemy)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyPoolManager: No free enemy in pool"));
        return nullptr;
    }

    // Move to summon location
    FVector TargetLocation = SummonMarker ? SummonMarker->GetActorLocation() : GetActorLocation();
    Enemy->SetActorLocation(TargetLocation);
    Enemy->ActivateCube();

    // If you want to initialize behavior, you'd call something here, or run a spawn animation

    return Enemy;
}

void AEnemyPoolManager::ReturnToPool(AEnemyCube* Enemy)
{
    if (!Enemy) return;
    // Move back and deactivate
    if (PoolMarker)
    {
        Enemy->SetActorLocation(PoolMarker->GetActorLocation());
    }
    Enemy->DeactivateCube();
}

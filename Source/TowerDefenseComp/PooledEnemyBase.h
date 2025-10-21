// Fill out your copyright notice in the Description page of Project Settings.

// PooledEnemyBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PooledEnemyBase.generated.h"

class APooledEnemySpawner;

UCLASS()
class YOURGAME_API APooledEnemyBase : public ACharacter
{
    GENERATED_BODY()

public:
    APooledEnemyBase();

    /** Set by the spawner after spawn so enemy can return itself to the pool. */
    void SetOwningSpawner(APooledEnemySpawner* InSpawner) { OwningSpawner = InSpawner; }

    /** Called by spawner to put this enemy into the pool (hidden / inert). */
    UFUNCTION()
    virtual void PrepareForPool();

    /** Called by spawner to activate this enemy (unhide / enable AI/movement). */
    UFUNCTION()
    virtual void ActivateFromPool(const FTransform& SpawnTransform);

    /** Call this when the enemy actually 'dies' (health <= 0). Returns to pool. */
    UFUNCTION()
    virtual void ReturnToPool();

protected:
    virtual void BeginPlay() override;

    /** Not owned; spawner manages lifetime */
    UPROPERTY()
    AEnemySpawner* OwningSpawner = nullptr;

    /** True when active in the world (not pooled). */
    UPROPERTY()
    bool bIsActive = false;
};

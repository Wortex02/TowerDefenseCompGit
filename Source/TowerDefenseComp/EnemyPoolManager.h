// Fill out your copyright notice in the Description page of Project Settings.

/*#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyPoolManager.generated.h"

class AEnemyCube;

UCLASS()
class TOWERDEFENSECOMP_API AEnemyPoolManager : public AActor
{
    GENERATED_BODY()

public:
    AEnemyPoolManager();

    // Enemy class to pool (set in editor)
    UPROPERTY(EditAnywhere, Category = "Pool")
    TSubclassOf<AEnemyCube> EnemyClass;

    // Pre-allocate this many
    UPROPERTY(EditAnywhere, Category = "Pool", meta = (ClampMin = 0))
    int32 InitialPoolSize = 20;

    // Add new spawns if pool empty?
    UPROPERTY(EditAnywhere, Category = "Pool")
    bool bAllowGrow = true;

    // Acquire an enemy from the pool. If a pooled inactive one exists it will be reused.
    AEnemyCube* GetPooledEnemy();

    // Return an enemy to the pool (call from enemy when "dead")
    void ReturnToPool(AEnemyCube* Enemy);

protected:
    virtual void BeginPlay() override;

    // Internal storage of pooled actors
    UPROPERTY()
    TArray<AEnemyCube*> Pool;

    // Location where returned actors are placed (optional)
    UPROPERTY(EditAnywhere, Category = "Pool")
    FVector PoolLocation = FVector::ZeroVector;

    // Helper
    void PreFillPool();
    AEnemyCube* SpawnAndPrepare();
};*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyPoolManager.generated.h"

class AEnemyCube;
class ATargetPoint;

UCLASS()
class TOWERDEFENSECOMP_API AEnemyPoolManager : public AActor
{
    GENERATED_BODY()

public:
    AEnemyPoolManager();

    virtual void BeginPlay() override;

    // Fill pool at BeginPlay or on demand
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool")
    int32 PoolSize = 10;

    // Enemy to spawn
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool")
    TSubclassOf<AEnemyCube> EnemyClass;

    // Where pooled enemies initially sit (use TargetPoint in level)
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Pool")
    ATargetPoint* PoolMarker;

    // Where to place an enemy when summoned
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Pool")
    ATargetPoint* SummonMarker;

    // Summon via Blueprint or C++
    UFUNCTION(BlueprintCallable, Category = "Pool")
    AEnemyCube* SummonFromPool();

    // Return to pool (can be called by enemy)
    UFUNCTION(BlueprintCallable, Category = "Pool")
    void ReturnToPool(AEnemyCube* Enemy);

protected:
    UPROPERTY()
    TArray<AEnemyCube*> Pool;

    // Helper: get an inactive one
    AEnemyCube* GetInactiveEnemy();
};



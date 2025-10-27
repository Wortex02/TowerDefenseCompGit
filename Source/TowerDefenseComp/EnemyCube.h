// Fill out your copyright notice in the Description page of Project Settings.

/*#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyCube.generated.h"

UCLASS()
class TOWERDEFENSECOMP_API AEnemyCube : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};*/

/*#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
//#include "AIController.h"
#include "EnemyCube.generated.h"

class AEnemyPoolManager;
class AAIController;

UCLASS()
class TOWERDEFENSECOMP_API AEnemyCube : public APawn
{
    GENERATED_BODY()

public:
    AEnemyCube();

    // Back reference to pool manager (set by pool)
    UPROPERTY(Transient)
    AEnemyPoolManager* OwningPool = nullptr;

    // Movement component for simple velocity-based movement
    UPROPERTY(VisibleAnywhere)
    class UFloatingPawnMovement* FloatingMovement;

    // Called by pool manager when actor is activated for reuse
    virtual void ResetForReuse();

    // Called by pool manager (or self) when it should be returned to pool
    virtual void CleanupBeforePool();

    // Call this when enemy dies to return to pool
    void ReturnToPool();

    // Simple API: set path/waypoints for this spawn
    void InitializePath(const TArray<FVector>& InWaypoints);

protected:
    virtual void BeginPlay() override;

    // Waypoint driving state (simple example)
    TArray<FVector> Waypoints;
    int32 CurrentWaypointIndex = 0;

    void TickMovement(float Delta);
    virtual void Tick(float Delta) override;
};*/


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyCube.generated.h"

UCLASS()
class TOWERDEFENSECOMP_API AEnemyCube : public AActor
{
    GENERATED_BODY()

public:
    AEnemyCube();

    // Called to make cube active in game (summoned)
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    virtual void ActivateCube();

    // Called to deactivate and return to pool
    UFUNCTION(BlueprintCallable, Category = "Enemy")
    virtual void DeactivateCube();

    // Optional: notify PoolManager when it's done (e.g. on "death")
    UPROPERTY(BlueprintReadWrite, Category = "Enemy")
    AActor* OwningPoolManager;

protected:
    virtual void BeginPlay() override;

    // whether currently active
    bool bIsActive;
};

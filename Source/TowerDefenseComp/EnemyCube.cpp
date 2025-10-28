// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCube.h"

// Sets default values
AEnemyCube::AEnemyCube()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCube::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/*#include "EnemyCube.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "EnemyPoolManager.h"
#include "GameFramework/FloatingPawnMovement.h"

AEnemyCube::AEnemyCube()
{
    PrimaryActorTick.bCanEverTick = true;

    FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMovement"));
    FloatingMovement->UpdatedComponent = GetRootComponent();
}

void AEnemyCube::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyCube::ResetForReuse()
{
    // Reset health, status effects, animation state etc.
    CurrentWaypointIndex = 0;
    Waypoints.Empty();
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);

    // If possessed by AIController, restart logic or call AI init here
    if (AAIController* AI = Cast<AAIController>(GetController()))
    {
        // Example: restart behavior tree or custom AI logic
        // AI->RunBehaviorTree(YourBTAsset) or call a custom method
    }
}

void AEnemyCube::CleanupBeforePool()
{
    // Stop movement & AI, clear timers, stop effects
    if (UFloatingPawnMovement* FM = FloatingMovement)
    {
        FM->StopMovementImmediately();
    }

    if (AAIController* AI = Cast<AAIController>(GetController()))
    {
        // Stop AI logic while pooled
        if (AI->BrainComponent)
        {
            AI->BrainComponent->StopLogic("Pooling");
        }
        AI->UnPossess(); // optionally unpossess to free controller
    }

    // Hide and disable
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);
}

void AEnemyCube::ReturnToPool()
{
    if (OwningPool)
    {
        OwningPool->ReturnToPool(this);
    }
    else
    {
        Destroy();
    }
}

void AEnemyCube::InitializePath(const TArray<FVector>& InWaypoints)
{
    Waypoints = InWaypoints;
    CurrentWaypointIndex = 0;
}

void AEnemyCube::TickMovement(float Delta)
{
    if (Waypoints.Num() == 0) return;
    if (CurrentWaypointIndex >= Waypoints.Num()) return;

    FVector Target = Waypoints[CurrentWaypointIndex];
    FVector ToTarget = Target - GetActorLocation();
    float Dist = ToTarget.Size();
    if (Dist < 10.f) // reached
    {
        CurrentWaypointIndex++;
        if (CurrentWaypointIndex >= Waypoints.Num())
        {
            // reached end -> return to pool or trigger arrive logic
            ReturnToPool();
            return;
        }
        Target = Waypoints[CurrentWaypointIndex];
        ToTarget = Target - GetActorLocation();
    }

    FVector Dir = ToTarget.GetSafeNormal();
    if (FloatingMovement)
    {
        FloatingMovement->AddInputVector(Dir);
    }
    else
    {
        // fallback manual move
        FVector NewLoc = GetActorLocation() + Dir * 300.f * Delta;
        SetActorLocation(NewLoc);
    }
}

void AEnemyCube::Tick(float Delta)
{
    Super::Tick(Delta);
    TickMovement(Delta);
}*/


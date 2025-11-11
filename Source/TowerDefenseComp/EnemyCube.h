// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyCube.generated.h"

class UAStarPathfinder;

UCLASS()
class TOWERDEFENSECOMP_API AEnemyCube : public APawn
{
    GENERATED_BODY()

public:
    AEnemyCube();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // Visual
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CubeMesh;

    // move speed (units per second)
    UPROPERTY(EditAnywhere, Category = "Movement")
    float MoveSpeed = 300.f;

    // path and follow index
    TArray<FVector> CurrentPath;
    int32 PathIndex = 0;

    UPROPERTY(VisibleAnywhere)
    UAStarPathfinder* PathfinderComponent;

    // tag to find the goal Actor in level
    UPROPERTY(EditAnywhere, Category = "AStar")
    FName GoalTag = FName("Goal");

    // distance threshold to snap to a waypoint
    UPROPERTY(EditAnywhere, Category = "Movement")
    float WaypointTolerance = 20.f;

    void RequestPathToGoal();
};

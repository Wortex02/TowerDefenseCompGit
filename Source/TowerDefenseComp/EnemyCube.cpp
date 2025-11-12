// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCube.h"
#include "Components/StaticMeshComponent.h"
#include "AStarPathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AEnemyCube::AEnemyCube()
{
    PrimaryActorTick.bCanEverTick = true;
    PathfinderComponent = CreateDefaultSubobject<UAStarPathfinder>(TEXT("AStarPathfinder"));

    CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
    RootComponent = CubeMesh;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (MeshAsset.Succeeded())
    {
        CubeMesh->SetStaticMesh(MeshAsset.Object);
    }
}

void AEnemyCube::BeginPlay()
{
    Super::BeginPlay();

    PathfinderComponent->BuildGrid();

    RequestPathToGoal();
}

void AEnemyCube::RequestPathToGoal()
{
    UWorld* W = GetWorld();
    if (!W) return;

    TArray<AActor*> Goals;
    UGameplayStatics::GetAllActorsWithTag(W, GoalTag, Goals);
    if (Goals.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyCube: No actor found with tag 'Goal'"));
        return;
    }

    FVector goalLocation = Goals[0]->GetActorLocation();
    FVector startLocation = GetActorLocation();

    TArray<FVector> Path;
    if (PathfinderComponent->FindPath(startLocation, goalLocation, Path))
    {
        CurrentPath = Path;
        PathIndex = 0;
        UE_LOG(LogTemp, Log, TEXT("EnemyCube: Path found with %d waypoints"), CurrentPath.Num());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyCube: No path found"));
    }
}

void AEnemyCube::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentPath.Num() == 0 || PathIndex >= CurrentPath.Num()) return;

    FVector myLoc = GetActorLocation();
    FVector target = CurrentPath[PathIndex];

    // move toward target in XY plane, preserve Z
    FVector toTarget = target - myLoc;
    toTarget.Z = 0;
    float dist = toTarget.Size();
    if (dist <= WaypointTolerance)
    {
        // snap and advance
        SetActorLocation(FVector(target.X, target.Y, myLoc.Z));
        PathIndex++;
        return;
    }

    FVector dir = toTarget.GetSafeNormal();
    FVector newLoc = myLoc + dir * MoveSpeed * DeltaTime;
    SetActorLocation(newLoc);
}

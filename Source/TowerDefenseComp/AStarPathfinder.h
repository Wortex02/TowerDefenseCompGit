// Fill out your copyright notice in the Description page of Project Settings.

/*#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AStarPathfinder.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TOWERDEFENSECOMP_API UAStarPathfinder : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAStarPathfinder();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};*/

// AStarPathfinder.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AStarPathfinder.generated.h"

USTRUCT()
struct FGridNode
{
    GENERATED_BODY()

    FIntPoint Coord = FIntPoint::ZeroValue;
    bool bBlocked = false;
    float G = TNumericLimits<float>::Max();
    float H = 0.f;
    FIntPoint Parent = FIntPoint::ZeroValue;

    float F() const { return G + H; }
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TOWERDEFENSECOMP_API UAStarPathfinder : public UActorComponent
{
    GENERATED_BODY()

public:
    UAStarPathfinder();

    // Grid config
    UPROPERTY(EditAnywhere, Category = "AStar|Grid")
    FVector GridOrigin = FVector::ZeroVector; // world-space origin (bottom-left) of the grid

    UPROPERTY(EditAnywhere, Category = "AStar|Grid")
    int32 GridWidth = 50; // number of columns

    UPROPERTY(EditAnywhere, Category = "AStar|Grid")
    int32 GridHeight = 50; // number of rows

    UPROPERTY(EditAnywhere, Category = "AStar|Grid")
    float CellSize = 100.f; // world size per cell (units)

    // tag used to detect obstacles
    UPROPERTY(EditAnywhere, Category = "AStar|Grid")
    FName ObstacleTag = FName("PathObstacle");

    // Build or rebuild the grid (calls should be done before FindPath)
    UFUNCTION(CallInEditor, BlueprintCallable, Category = "AStar")
    void BuildGrid();

    // Find a path; returns true if path found and fills OutPath with world positions (cell centers)
    UFUNCTION(BlueprintCallable, Category = "AStar")
    bool FindPath(const FVector& StartWorld, const FVector& GoalWorld, TArray<FVector>& OutPath);

protected:
    virtual void BeginPlay() override;

private:
    TMap<FIntPoint, FGridNode> Nodes;

    bool WorldToGrid(const FVector& World, FIntPoint& OutCoord) const;
    FVector GridToWorldCenter(const FIntPoint& Coord) const;
    void MakeNode(const FIntPoint& Coord, bool bBlocked);
    void GatherObstacleBoxes(TArray<FBox>& OutBoxes) const;
};


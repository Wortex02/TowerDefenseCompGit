// Fill out your copyright notice in the Description page of Project Settings.


/*#include "AStarPathfinder.h"

// Sets default values for this component's properties
UAStarPathfinder::UAStarPathfinder()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAStarPathfinder::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAStarPathfinder::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}*/

// AStarPathfinder.cpp
#include "AStarPathfinder.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"

UAStarPathfinder::UAStarPathfinder()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAStarPathfinder::BeginPlay()
{
    Super::BeginPlay();
    // Optionally auto-build on play
    BuildGrid();
}

void UAStarPathfinder::BuildGrid()
{
    Nodes.Empty();

    // gather obstacle bounding boxes (actors with tag)
    TArray<FBox> ObBoxes;
    GatherObstacleBoxes(ObBoxes);

    for (int32 y = 0; y < GridHeight; ++y)
    {
        for (int32 x = 0; x < GridWidth; ++x)
        {
            FIntPoint coord(x, y);
            // compute cell world AABB
            FVector cellCenter = GridToWorldCenter(coord);
            FVector half(CellSize * 0.5f, CellSize * 0.5f, 10000.f); // z large enough to cover actor heights
            FBox cellBox = FBox::BuildAABB(cellCenter, half);

            bool bBlocked = false;
            for (const FBox& b : ObBoxes)
            {
                if (b.Intersect(cellBox))
                {
                    bBlocked = true;
                    break;
                }
            }
            MakeNode(coord, bBlocked);
        }
    }
}

void UAStarPathfinder::GatherObstacleBoxes(TArray<FBox>& OutBoxes) const
{
    OutBoxes.Empty();
    UWorld* W = GetWorld();
    if (!W) return;

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsWithTag(W, ObstacleTag, Found);
    for (AActor* A : Found)
    {
        if (!A) continue;
        OutBoxes.Add(A->GetComponentsBoundingBox(true));
    }
}

void UAStarPathfinder::MakeNode(const FIntPoint& Coord, bool bBlocked)
{
    FGridNode Node;
    Node.Coord = Coord;
    Node.bBlocked = bBlocked;
    Node.G = TNumericLimits<float>::Max();
    Node.H = 0.f;
    Node.Parent = FIntPoint(-1, -1);

    Nodes.Add(Coord, Node);
}

bool UAStarPathfinder::WorldToGrid(const FVector& World, FIntPoint& OutCoord) const
{
    FVector rel = World - GridOrigin;
    int32 x = FMath::FloorToInt(rel.X / CellSize);
    int32 y = FMath::FloorToInt(rel.Y / CellSize);
    if (x < 0 || x >= GridWidth || y < 0 || y >= GridHeight) return false;
    OutCoord = FIntPoint(x, y);
    return true;
}

FVector UAStarPathfinder::GridToWorldCenter(const FIntPoint& Coord) const
{
    float cx = GridOrigin.X + (Coord.X + 0.5f) * CellSize;
    float cy = GridOrigin.Y + (Coord.Y + 0.5f) * CellSize;
    // use origin Z
    return FVector(cx, cy, GridOrigin.Z);
}

bool UAStarPathfinder::FindPath(const FVector& StartWorld, const FVector& GoalWorld, TArray<FVector>& OutPath)
{
    OutPath.Empty();

    FIntPoint start, goal;
    if (!WorldToGrid(StartWorld, start) || !WorldToGrid(GoalWorld, goal))
    {
        UE_LOG(LogTemp, Warning, TEXT("AStar: start or goal outside grid"));
        return false;
    }

    if (!Nodes.Contains(start) || !Nodes.Contains(goal))
    {
        return false;
    }
    if (Nodes[start].bBlocked || Nodes[goal].bBlocked)
    {
        // you might want to allow endpoint on blocked (e.g. goal actor sits on obstacle) - adjust here
        UE_LOG(LogTemp, Warning, TEXT("AStar: start or goal blocked"));
        return false;
    }

    // reset nodes G/H/Parent
    for (auto& Pair : Nodes)
    {
        Pair.Value.G = TNumericLimits<float>::Max();
        Pair.Value.Parent = FIntPoint(-1, -1);
    }

    auto Heuristic = [&](const FIntPoint& A, const FIntPoint& B)
        {
            // Euclidean
            float dx = (A.X - B.X);
            float dy = (A.Y - B.Y);
            return FMath::Sqrt(dx * dx + dy * dy) * CellSize;
        };

    // open set as array of coords (simple linear search for min F) — OK for moderate grid sizes
    TArray<FIntPoint> Open;
    TSet<FIntPoint> Closed;

    Nodes[start].G = 0.f;
    Nodes[start].H = Heuristic(start, goal);
    Open.Add(start);

    const TArray<FIntPoint> Neighbors = {
        FIntPoint(1,0), FIntPoint(-1,0), FIntPoint(0,1), FIntPoint(0,-1),
        FIntPoint(1,1), FIntPoint(-1,1), FIntPoint(1,-1), FIntPoint(-1,-1)
    };

    while (Open.Num() > 0)
    {
        // find node in Open with smallest F
        int32 bestIdx = 0;
        float bestF = TNumericLimits<float>::Max();
        for (int32 i = 0; i < Open.Num(); ++i)
        {
            const FGridNode& n = Nodes[Open[i]];
            float f = n.F();
            if (f < bestF) { bestF = f; bestIdx = i; }
        }
        FIntPoint current = Open[bestIdx];
        Open.RemoveAtSwap(bestIdx);

        if (current == goal)
        {
            // reconstruct path
            TArray<FIntPoint> rev;
            FIntPoint cur = current;
            while (cur.X >= 0 && cur.Y >= 0)
            {
                rev.Add(cur);
                cur = Nodes[cur].Parent;
            }
            // reverse to start->goal and convert to world centers
            for (int32 i = rev.Num() - 1; i >= 0; --i)
            {
                OutPath.Add(GridToWorldCenter(rev[i]));
            }
            return true;
        }

        Closed.Add(current);

        for (const FIntPoint& dir : Neighbors)
        {
            FIntPoint nb = FIntPoint(current.X + dir.X, current.Y + dir.Y);
            if (!Nodes.Contains(nb)) continue;
            FGridNode& nbNode = Nodes[nb];
            if (nbNode.bBlocked) continue;
            if (Closed.Contains(nb)) continue;

            // movement cost: diagonal cost = sqrt(2) * cellsize
            float moveCost = (dir.X != 0 && dir.Y != 0) ? (FMath::Sqrt(2.0f) * CellSize) : CellSize;
            float tentativeG = Nodes[current].G + moveCost;

            if (tentativeG < nbNode.G)
            {
                nbNode.G = tentativeG;
                nbNode.H = Heuristic(nb, goal);
                nbNode.Parent = current;
                if (!Open.Contains(nb))
                {
                    Open.Add(nb);
                }
            }
        }
    }

    // no path
    return false;
}



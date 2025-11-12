#include "AGridManager.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	DrawGrid();
}

void AGridManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	DrawGrid();
}

void AGridManager::DrawGrid() const
{
	if (!GetWorld()) return;

	const float Z = GridZ;
	const float W = CellsX * GridSize;
	const float H = CellsY * GridSize;
	
	for (int32 x = 0; x <= CellsX; ++x)
	{
		const float X = Origin.X + x * GridSize;
		const FVector A(X, Origin.Y, Z);
		const FVector B(X, Origin.Y + H, Z);
		DrawDebugLine(GetWorld(), A, B, LineColor, true, 0.f, 0, LineThickness);
	}

	for (int32 y = 0; y <= CellsY; ++y)
	{
		const float Y = Origin.Y + y * GridSize;
		const FVector A(Origin.X, Y, Z);
		const FVector B(Origin.X + W, Y, Z);
		DrawDebugLine(GetWorld(), A, B, LineColor, true, 0.f, 0, LineThickness);
	}
}

bool AGridManager::IsInside(const FIntPoint& Cell) const
{
	return Cell.X >= 0 && Cell.Y >= 0 && Cell.X < CellsX && Cell.Y < CellsY;
}

FIntPoint AGridManager::WorldToCell(const FVector& WorldPos) const
{
	const float LocalX = (WorldPos.X - Origin.X) / GridSize;
	const float LocalY = (WorldPos.Y - Origin.Y) / GridSize;
	return FIntPoint(FMath::FloorToInt(LocalX), FMath::FloorToInt(LocalY));
}

FVector AGridManager::CellToWorld(const FIntPoint& Cell) const
{
	const float X = Origin.X + (Cell.X + 0.5f) * GridSize;
	const float Y = Origin.Y + (Cell.Y + 0.5f) * GridSize;
	return FVector(X, Y, GridZ);
}

FVector AGridManager::SnapToGrid(const FVector& WorldPos) const
{
	return CellToWorld(WorldToCell(WorldPos));
}

bool AGridManager::IsCellFree(const FIntPoint& Cell) const
{
	return IsInside(Cell) && !Occupied.Contains(Cell);
}

AActor* AGridManager::TryPlaceAtWorld(UClass* ActorClass, const FVector& WorldPos)
{
	if (!ActorClass || !GetWorld()) return nullptr;

	const FIntPoint Cell = WorldToCell(WorldPos);
	if (!IsCellFree(Cell)) return nullptr;

	const FVector SpawnLoc = CellToWorld(Cell);
	const FRotator SpawnRot = FRotator::ZeroRotator;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ActorClass, SpawnLoc, SpawnRot, Params);
	if (Spawned)
	{
		Occupied.Add(Cell);
	}
	return Spawned;
}

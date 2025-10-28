#include "AGridManager.h"

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

FVector AGridManager::SnapToGrid(const FVector& WorldPos) const
{
    const FVector Local = WorldPos - Origin;
    const float Gx = FMath::RoundToFloat(Local.X / GridSize) * GridSize;
    const float Gy = FMath::RoundToFloat(Local.Y / GridSize) * GridSize;
    return Origin + FVector(Gx, Gy, WorldPos.Z);
}

FIntPoint AGridManager::WorldToCell(const FVector& WorldPos) const
{
	const FVector Local = WorldPos - Origin;

	const int32 CX = FMath::RoundToInt(Local.X / GridSize);
	const int32 CY = FMath::RoundToInt(Local.Y / GridSize);

	return FIntPoint(CX, CY);
}

FVector AGridManager::CellToWorld(const FIntPoint& Cell, float Z) const
{
	return Origin + FVector(Cell.X * GridSize, Cell.Y * GridSize, Z);
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


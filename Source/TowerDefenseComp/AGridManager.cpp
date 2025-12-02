#include "AGridManager.h"
#include "DrawDebugHelpers.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/World.h"

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = false;
	
	GridLineMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("GridLineMesh"));
	RootComponent = GridLineMesh;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		GridLineMesh->SetStaticMesh(CubeMesh.Object);
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(TEXT("/Script/Engine.Material'/Game/cyan.cyan'"));
	if (MatFinder.Succeeded())
	{
		GridLineMesh->SetMaterial(0, MatFinder.Object);
	}
	
	GridLineMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
	//DrawGrid();
	
	BuildGrid();
}

void AGridManager::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	//DrawGrid();
	
	BuildGrid();
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
void AGridManager::BuildGrid()
{
	if (!GridLineMesh) return;

	GridLineMesh->ClearInstances();

	const float Z = GridZ;
	const float W = CellsX * GridSize;
	const float H = CellsY * GridSize;

	// Base cube in UE is 100x100x100 cm
	float BaseSize = 100.f;

	// VERTICAL LINES (along Y)
	for (int32 x = 0; x <= CellsX; ++x)
	{
		const float X = Origin.X + x * GridSize;
		const FVector Start(X, Origin.Y,     Z);
		const FVector End  (X, Origin.Y + H, Z);

		const FVector LineCenter = (Start + End) * 0.5f;
		const float   LineLength = (End - Start).Size();

		FTransform InstanceTransform;
		InstanceTransform.SetLocation(LineCenter);

		// Oriented along +Y (no yaw rotation needed if cube's Y axis is "forward")
		const FRotator Rot(0.f, 0.f, 0.f);
		InstanceTransform.SetRotation(Rot.Quaternion());

		// X/Z = thickness, Y = length
		const float ThicknessScale = LineThickness / BaseSize;
		const float LengthScale    = LineLength    / BaseSize;

		const FVector Scale(ThicknessScale, LengthScale, ThicknessScale);
		InstanceTransform.SetScale3D(Scale);

		GridLineMesh->AddInstance(InstanceTransform);
	}

	// HORIZONTAL LINES (along X)
	for (int32 y = 0; y <= CellsY; ++y)
	{
		const float Y = Origin.Y + y * GridSize;
		const FVector Start(Origin.X,     Y, Z);
		const FVector End  (Origin.X + W, Y, Z);

		const FVector LineCenter = (Start + End) * 0.5f;
		const float   LineLength = (End - Start).Size();

		FTransform InstanceTransform;
		InstanceTransform.SetLocation(LineCenter);

		// Rotate 90 degrees so it runs along X instead of Y
		const FRotator Rot(0.f, 90.f, 0.f);
		InstanceTransform.SetRotation(Rot.Quaternion());

		const float ThicknessScale = LineThickness / BaseSize;
		const float LengthScale    = LineLength    / BaseSize;

		const FVector Scale(ThicknessScale, LengthScale, ThicknessScale);
		InstanceTransform.SetScale3D(Scale);

		GridLineMesh->AddInstance(InstanceTransform);
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

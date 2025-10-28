#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AGridManager.generated.h"

UCLASS()
class AGridManager : public AActor
{
	GENERATED_BODY()

public:
	AGridManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	FVector Origin = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	float GridSize = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	int32 CellsX = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	int32 CellsY = 20;

	// Z where we draw and snap (your “ground” height)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	float GridZ = 0.f;

	// Debug visual
	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	FColor LineColor = FColor::Cyan;

	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	float LineThickness = 5.0f;

	// Convert world <-> grid
	UFUNCTION(BlueprintCallable, Category="Grid")
	FIntPoint WorldToCell(const FVector& WorldPos) const;

	UFUNCTION(BlueprintCallable, Category="Grid")
	FVector CellToWorld(const FIntPoint& Cell) const;

	UFUNCTION(BlueprintCallable, Category="Grid")
	FVector SnapToGrid(const FVector& WorldPos) const;

	// Occupancy
	UFUNCTION(BlueprintCallable, Category="Grid")
	bool IsCellFree(const FIntPoint& Cell) const;

	// Try to place actor at a world position (snapped). Returns spawned actor or nullptr.
	UFUNCTION(BlueprintCallable, Category="Grid")
	AActor* TryPlaceAtWorld(UClass* ActorClass, const FVector& WorldPos);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

private:
	// Occupied cells
	TSet<FIntPoint> Occupied;

	// Internal: draw the grid lines
	void DrawGrid() const;

	// Bounds check
	bool IsInside(const FIntPoint& Cell) const;
};


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
	int CellsX = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	int CellsY = 20;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	float GridZ = 0.f;
	
	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	FColor LineColor = FColor::Cyan;

	UPROPERTY(EditAnywhere, Category="Grid|Debug")
	float LineThickness = 5.0f;
	
	UFUNCTION(BlueprintCallable, Category="Grid")
	FIntPoint WorldToCell(const FVector& WorldPos) const;

	UFUNCTION(BlueprintCallable, Category="Grid")
	FVector CellToWorld(const FIntPoint& Cell) const;

	UFUNCTION(BlueprintCallable, Category="Grid")
	FVector SnapToGrid(const FVector& WorldPos) const;
	
	UFUNCTION(BlueprintCallable, Category="Grid")
	bool IsCellFree(const FIntPoint& Cell) const;
	
	UFUNCTION(BlueprintCallable, Category="Grid")
	AActor* TryPlaceAtWorld(UClass* ActorClass, const FVector& WorldPos);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

private:
	TSet<FIntPoint> Occupied;
	
	void DrawGrid() const;
	
	bool IsInside(const FIntPoint& Cell) const;
};


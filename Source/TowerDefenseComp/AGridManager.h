// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AGridManager.generated.h"

UCLASS()
class TOWERDEFENSECOMP_API AGridManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AGridManager();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	float GridSize = 200.f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Grid")
	FVector Origin = FVector::ZeroVector;
    
	UFUNCTION(BlueprintCallable, Category="Grid")
	FVector SnapToGrid(const FVector& WorldPos) const;
	
	bool IsCellFree(const FIntPoint& Cell) const { return true; }
	FIntPoint WorldToCell(const FVector& WorldPos) const;
	FVector   CellToWorld(const FIntPoint& Cell, float Z) const;
		
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};

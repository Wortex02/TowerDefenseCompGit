// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnPoint.generated.h"

UCLASS()
class TOWERDEFENSECOMP_API AEnemySpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnPoint();

	// Optional lane index or tag to identify this spawn
	UPROPERTY(EditInstanceOnly, Category = "Spawn")
	int32 LaneIndex = 0;

	// Optional lateral offset vector for formation anchoring
	UPROPERTY(EditInstanceOnly, Category = "Spawn")
	FVector LocalOffset = FVector::ZeroVector;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

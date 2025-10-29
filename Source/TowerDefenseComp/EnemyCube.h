// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyCube.generated.h"

UCLASS()
class TOWERDEFENSECOMP_API AEnemyCube : public APawn
{
    GENERATED_BODY()

public:
    AEnemyCube();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // Visual
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CubeMesh;
};

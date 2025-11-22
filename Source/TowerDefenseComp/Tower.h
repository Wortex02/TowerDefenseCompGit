// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tower.generated.h"

UCLASS()
class TOWERDEFENSECOMP_API ATower : public AActor
{
    GENERATED_BODY()

public:
    ATower();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    void Shoot();

    // Components
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* TowerMesh;

    UPROPERTY(VisibleAnywhere)
    class USphereComponent* RangeSphere;

    // Projectile
    UPROPERTY(EditAnywhere, Category = "Combat")
    TSubclassOf<class AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float FireRate = 1.f;

private:
    float LastFireTime = 0.f;
};

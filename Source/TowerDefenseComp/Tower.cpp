// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "Projectile.h"
#include "EnemyCube.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ATower::ATower()
{
    PrimaryActorTick.bCanEverTick = true;

    TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh"));
    RootComponent = TowerMesh;

    RangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RangeSphere"));
    RangeSphere->SetupAttachment(RootComponent);
    RangeSphere->SetSphereRadius(1000.f);
}

void ATower::BeginPlay()
{
    Super::BeginPlay();
}

void ATower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!ProjectileClass) return;

    // Find nearest enemy in range
    TArray<AActor*> OverlappingActors;
    RangeSphere->GetOverlappingActors(OverlappingActors, AEnemyCube::StaticClass());

    if (OverlappingActors.Num() > 0)
    {
        float CurrentTime = GetWorld()->GetTimeSeconds();
        if (CurrentTime - LastFireTime >= FireRate)
        {
            LastFireTime = CurrentTime;
            Shoot();
        }
    }
}

void ATower::Shoot()
{
    TArray<AActor*> OverlappingActors;
    RangeSphere->GetOverlappingActors(OverlappingActors, AEnemyCube::StaticClass());

    if (OverlappingActors.Num() > 0)
    {
        AActor* Target = OverlappingActors[0];
        FVector SpawnLocation = TowerMesh->GetComponentLocation() + FVector(0, 0, 50.f);
        FRotator SpawnRotation = (Target->GetActorLocation() - SpawnLocation).Rotation();

        AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
        UE_LOG(LogTemp, Warning, TEXT("Tower %s fired at %s"), *GetName(), *Target->GetName());
        UE_LOG(LogTemp, Warning, TEXT("Projectile Fired!"));
    }
}



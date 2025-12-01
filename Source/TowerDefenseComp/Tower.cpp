// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "Projectile.h"
#include "Enemy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

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
    InitializeProjectilePool();
}

void ATower::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!ProjectileClass) return;

    TArray<AActor*> OverlappingActors;
    RangeSphere->GetOverlappingActors(OverlappingActors, AEnemy::StaticClass());

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

void ATower::InitializeProjectilePool()
{
    if (!ProjectileClass) return;

    for (int32 i = 0; i < PoolSize; i++)
    {
        AProjectile* Proj = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, FVector::ZeroVector, FRotator::ZeroRotator);
        if (Proj)
        {
            Proj->Deactivate();
            ProjectilePool.Add(Proj);
        }
    }
}

AProjectile* ATower::GetPooledProjectile()
{
    for (AProjectile* Proj : ProjectilePool)
    {
        if (Proj && !Proj->bActive)
        {
            return Proj;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Tower has no free projectiles in pool!"));
    return nullptr;
}

void ATower::Shoot()
{
    TArray<AActor*> OverlappingActors;
    RangeSphere->GetOverlappingActors(OverlappingActors, AEnemy::StaticClass());

    if (OverlappingActors.Num() == 0) return;

    AEnemy* Target = Cast<AEnemy>(OverlappingActors[0]);
    if (!Target) return;

    AProjectile* Projectile = GetPooledProjectile();
    if (Projectile)
    {
        FVector SpawnLocation = TowerMesh->GetComponentLocation() + FVector(0, 0, 50.f);
        FRotator SpawnRotation = (Target->GetActorLocation() - SpawnLocation).Rotation();
        Projectile->SetActorLocation(SpawnLocation);
        Projectile->SetActorRotation(SpawnRotation);
        Projectile->Activate(Target);

        UE_LOG(LogTemp, Warning, TEXT("Tower %s fired at %s"), *GetName(), *Target->GetName());
    }
}




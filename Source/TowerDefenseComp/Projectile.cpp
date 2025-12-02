// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "EnemyCube.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/World.h"

AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    MeshComp->SetGenerateOverlapEvents(true);
    MeshComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlap);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 2000.f;
    ProjectileMovement->MaxSpeed = 2000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    InitialLifeSpan = 0.f;

    bActive = false;
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();
    Deactivate();
}

void AProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bActive && TargetEnemy)
    {
        if (!IsValid(TargetEnemy))
        {
            Deactivate();
            return;
        }

        FVector Dir = (TargetEnemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        ProjectileMovement->Velocity = Dir * ProjectileMovement->InitialSpeed;
    }
}

void AProjectile::Activate(AEnemyCube* Target)
{
    bActive = true;
    TargetEnemy = Target;

    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);

    UE_LOG(LogTemp, Warning, TEXT("Projectile ACTIVATED"));
}

void AProjectile::Deactivate()
{
    bActive = false;
    TargetEnemy = nullptr;

    ProjectileMovement->StopMovementImmediately();

    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    SetActorTickEnabled(false);
}

void AProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bActive) return;

    if (AEnemyCube* Enemy = Cast<AEnemyCube>(OtherActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Projectile HIT %s"), *Enemy->GetName());
        Enemy->TakeDamage(Damage);
        Deactivate(); // return to pool
    }
}

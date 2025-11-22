// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

AEnemy::AEnemy()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    // Enable collision
    MeshComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    MeshComp->SetNotifyRigidBodyCollision(true);
}

void AEnemy::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
}

void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AEnemy::TakeDamage(float DamageAmount)
{
    CurrentHealth -= DamageAmount;

    UE_LOG(LogTemp, Warning, TEXT("%s took %.1f damage, remaining %.1f"),
        *GetName(), DamageAmount, CurrentHealth);

    if (CurrentHealth <= 0.f)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s is destroyed!"), *GetName());
        Destroy();
    }
}


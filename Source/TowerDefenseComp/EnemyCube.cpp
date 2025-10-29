// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCube.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AEnemyCube::AEnemyCube()
{
    PrimaryActorTick.bCanEverTick = true;

    CubeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
    RootComponent = CubeMesh;

    // use the engine cube by default so you immediately see something
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (MeshAsset.Succeeded())
    {
        CubeMesh->SetStaticMesh(MeshAsset.Object);
    }
}

void AEnemyCube::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyCube::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    // For now do nothing — you can add movement later.
}

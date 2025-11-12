#include "ATestTower2.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AATestTower2::AATestTower2()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	Mesh->SetMobility(EComponentMobility::Static);
}


#include "ATestTower.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AATestTower::AATestTower()
{
	PrimaryActorTick.bCanEverTick = false;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	// Engine sphere
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		Mesh->SetStaticMesh(SphereMesh.Object);
	}
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));
	Mesh->SetMobility(EComponentMobility::Static);
}


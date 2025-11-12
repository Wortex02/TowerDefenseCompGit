#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATestTower.generated.h"

UCLASS()
class TOWERDEFENSECOMP_API AATestTower : public AActor
{
	GENERATED_BODY()
	
public:
	AATestTower();

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
};

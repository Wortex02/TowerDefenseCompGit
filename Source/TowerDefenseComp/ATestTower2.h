#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATestTower2.generated.h"

UCLASS()
class TOWERDEFENSECOMP_API AATestTower2 : public AActor
{
	GENERATED_BODY()
	
public:
	AATestTower2();

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;
};

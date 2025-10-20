#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "APlacementPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class AGridManager;

UCLASS()
class TOWERDEFENSECOMP_API APlacementPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="Input|Enhanced")
	TObjectPtr<UInputMappingContext> IMC_Default;

	UPROPERTY(EditAnywhere, Category="Input|Enhanced")
	TObjectPtr<UInputAction> IA_Place;
	
	UPROPERTY(EditAnywhere, Category="Placement")
	TSubclassOf<AActor> SphereClass;

	UPROPERTY(EditAnywhere, Category="Placement")
	TSoftObjectPtr<AGridManager> GridManager;

	UPROPERTY(EditAnywhere, Category="Camera")
	TSoftObjectPtr<ACameraActor> GameCamera;
protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	
private:
	void PlaceOnClick();
	bool GetGroundHit(FHitResult& OutHit) const;
};

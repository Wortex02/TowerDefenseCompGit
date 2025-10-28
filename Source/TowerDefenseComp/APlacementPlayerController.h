#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "APlacementPlayerController.generated.h"


class UInputMappingContext;
class UInputAction;
class AGridManager;

UCLASS()
class APlacementPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, Category="Input")
	TSoftObjectPtr<UInputMappingContext> InputMapping;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_Place = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* IA_P = nullptr;
	
	UPROPERTY(EditInstanceOnly, Category="Placement")
	TObjectPtr<AGridManager> Grid;
	
	UPROPERTY(EditDefaultsOnly, Category="Placement")
	TSubclassOf<AActor> PlaceableClass;

private:
	void OnPlaceTriggered(const struct FInputActionInstance& Instance);
	void PresedP();
	bool GetMouseHitOnGrid(FVector& OutHit) const;
	void ResolveGridManager();
};



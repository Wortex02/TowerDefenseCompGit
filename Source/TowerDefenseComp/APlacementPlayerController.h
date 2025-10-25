#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "APlacementPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class AGridManager;
class UUserWidget;

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

	// Set this in the Editor to the widget blueprint (your UCallEnemiesWidget-derived BP)
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> CallWidgetClass;

	UPROPERTY()
	UUserWidget* CallWidgetInstance;

	// Spawn configuration
	UPROPERTY(EditAnywhere, Category = "Spawning")
	int32 SpawnCount = 5;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	FVector SpawnOrigin = FVector(0.f, 0.f, 100.f);

	UPROPERTY(EditAnywhere, Category = "Spawning")
	float SpawnSpacing = 200.f;

	// Called by the widget to request spawning a wave
	UFUNCTION()
	void RequestSpawnEnemies(int32 CountOverride = -1);

	/*UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> CallWidgetClass;

	UPROPERTY()
	UUserWidget* CallWidgetInstance;*/


protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	
private:
	void PlaceOnClick();
	bool GetGroundHit(FHitResult& OutHit) const;
};

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShopWidget.h" 
#include "InputAction.h"
#include "InputMappingContext.h"
#include "APlacementPlayerController.generated.h"


class UInputMappingContext;
class UInputAction;
class AGridManager;
class UShopWidget;

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

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UShopWidget> ShopWidgetClass;
	
	UPROPERTY(EditAnywhere, Category="Money")
	int StartingMoney = 150;
	
	UPROPERTY(EditAnywhere, Category="Shop")
	TArray<FShopItem> ShopItems;

private:
	UPROPERTY() UShopWidget* ShopWidget = nullptr;
	int Money = 0;
	int SelectedIndex = INDEX_NONE;
	
	void OnPlaceTriggered(const struct FInputActionInstance& Instance);
	void PresedP();
	bool GetMouseHitOnGrid(FVector& OutHit) const;
	void ResolveGridManager();
	
	UFUNCTION()
	void OnShopItemClicked(int32 Index);
	
	bool CanAffordSelected() const;
	const FShopItem* GetSelectedItem() const;
	void UpdateUI();
};



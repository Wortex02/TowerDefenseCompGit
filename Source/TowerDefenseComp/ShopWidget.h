#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;

USTRUCT(BlueprintType)
struct FShopItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Cost = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> TowerClass = nullptr;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopItemClicked, int32, Index);

UCLASS()
class TOWERDEFENSECOMP_API UShopWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetItems(const TArray<FShopItem>& InItems);
	
	UFUNCTION(BlueprintCallable)
	void SetSelectedIndex(int InIndex);
	
	UFUNCTION(BlueprintCallable)
	void SetMoney(int NewMoney);
	
	UPROPERTY(BlueprintAssignable, Category="Shop")
	FOnShopItemClicked OnItemClicked;

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	UButton* TowerButton = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UButton* TowerButton_2 = nullptr;

	UPROPERTY(meta=(BindWidget))
	UImage* Outline = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	UImage* Outline_2 = nullptr;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* MoneyText = nullptr;
	
private:
	TArray<FShopItem> Items;
	int SelectedIndex = INDEX_NONE;

	UFUNCTION() void HandleClick0();
	UFUNCTION() void HandleClick1();
	void RefreshSelectionVisuals();
};


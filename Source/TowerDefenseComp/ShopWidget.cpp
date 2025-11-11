#include "ShopWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TowerButton)   TowerButton->OnClicked.AddDynamic(this, &UShopWidget::HandleClick0);
	if (TowerButton_2) TowerButton_2->OnClicked.AddDynamic(this, &UShopWidget::HandleClick1);
	
	if (Outline)   Outline->SetVisibility(ESlateVisibility::Hidden);
	if (Outline_2) Outline_2->SetVisibility(ESlateVisibility::Hidden);
}

void UShopWidget::SetItems(const TArray<FShopItem>& InItems) 
{
	Items = InItems;
}

void UShopWidget::SetSelectedIndex(int InIndex)
{
	SelectedIndex = InIndex;
	RefreshSelectionVisuals();
}

void UShopWidget::SetMoney(int NewMoney)
{
	if (MoneyText)
	{
		MoneyText->SetText(FText::FromString(FString::Printf(TEXT("%d$"), NewMoney)));
	}
}

void UShopWidget::HandleClick0() { OnItemClicked.Broadcast(0); }
void UShopWidget::HandleClick1() { OnItemClicked.Broadcast(1); }

void UShopWidget::RefreshSelectionVisuals()
{
	if (Outline)   Outline->SetVisibility(SelectedIndex == 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	if (Outline_2) Outline_2->SetVisibility(SelectedIndex == 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

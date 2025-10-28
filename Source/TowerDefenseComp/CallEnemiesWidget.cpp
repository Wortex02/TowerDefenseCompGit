// Fill out your copyright notice in the Description page of Project Settings.


#include "CallEnemiesWidget.h"
#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "APlacementPlayerController.h" // replace with your controller's header if different
#include "Kismet/GameplayStatics.h"

bool UCallEnemiesWidget::Initialize()
{
    if (!Super::Initialize()) return false;

    if (CallButton)
    {
        CallButton->OnClicked.AddDynamic(this, &UCallEnemiesWidget::OnCallButtonClicked);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UCallEnemiesWidget: CallButton is null (BindWidget failed). Make sure the UMG Button is named 'CallButton'"));
    }

    return true;
}

void UCallEnemiesWidget::OnCallButtonClicked()
{
    // Ask owning player controller to spawn the enemies
    if (APlayerController* PC = GetOwningPlayer())
    {
        if (APlacementPlayerController* MyPC = Cast<APlacementPlayerController>(PC))
        {
            MyPC->RequestSpawnEnemies();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("CallEnemiesWidget: Owning player controller is not APlacementPlayerController"));
        }
    }
}

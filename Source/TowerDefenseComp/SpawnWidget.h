// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpawnWidget.generated.h"

class UButton;

UCLASS()
class TOWERDEFENSECOMP_API USpawnWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

    // This requires the UMG Blueprint to have a Button named "SpawnButton"
    UPROPERTY(meta = (BindWidget))
    UButton* SpawnButton;

    UFUNCTION()
    void OnSpawnClicked();
};

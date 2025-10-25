// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CallEnemiesWidget.generated.h"

class UButton;

UCLASS()
class TOWERDEFENSECOMP_API UCallEnemiesWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

protected:
    // This expects a Button named "CallButton" in the UMG designer
    UPROPERTY(meta = (BindWidget))
    UButton* CallButton;

    UFUNCTION()
    void OnCallButtonClicked();
};


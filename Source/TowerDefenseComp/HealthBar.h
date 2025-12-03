// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class TOWERDEFENSECOMP_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	// Implement this in Blueprint (inside WBP_HealthBar)
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void ReduceHealth();
	
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "TowerDefenseGameMode.h"

void USpawnWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (SpawnButton)
    {
        SpawnButton->OnClicked.AddDynamic(this, &USpawnWidget::OnSpawnClicked);
    }
}

void USpawnWidget::OnSpawnClicked()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ATowerDefenseGameMode* GM = Cast<ATowerDefenseGameMode>(UGameplayStatics::GetGameMode(this));
    if (GM)
    {
        GM->SpawnEnemy();
    }
}



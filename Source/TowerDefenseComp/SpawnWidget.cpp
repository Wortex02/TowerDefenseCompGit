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

    // Get GameMode and bind to delegate
    if (GetWorld())
    {
        ATowerDefenseGameMode* GM = Cast<ATowerDefenseGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
        if (GM)
        {
            GM->OnSpawnStateChanged.AddDynamic(this, &USpawnWidget::HandleSpawnStateChanged);

            // Optionally set initial state immediately (in case GameMode already broadcast at BeginPlay)
            // We don't have a getter, so rely on the delegate broadcast: call UpdateSpawnState in GM's BeginPlay.
        }
    }
}

void USpawnWidget::OnSpawnClicked()
{
    UWorld* World = GetWorld();
    if (!World) return;

    ATowerDefenseGameMode* GM = Cast<ATowerDefenseGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (GM)
    {
        GM->SpawnEnemy(8);
    }
}

void USpawnWidget::HandleSpawnStateChanged(bool bCanSpawn)
{
    if (!SpawnButton) return;

    // Hide or show. You can use SetIsEnabled to simply disable instead.
    SpawnButton->SetVisibility(bCanSpawn ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

    // OR to gray out instead of hide:
    // SpawnButton->SetIsEnabled(bCanSpawn);
}

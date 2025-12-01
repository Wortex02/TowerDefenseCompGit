// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "TowerDefenseGameMode.h"

bool USpawnWidget::Initialize()
{
    if (!Super::Initialize()) return false;

    if (StartWaveButton)
    {
        StartWaveButton->OnClicked.AddDynamic(this, &USpawnWidget::OnStartWaveClicked);
    }

    return true;
}

void USpawnWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Find GameMode and cache it
    UWorld* World = GetWorld();
    if (!World) return;

    ATowerDefenseGameMode* GM = Cast<ATowerDefenseGameMode>(UGameplayStatics::GetGameMode(World));
    if (GM)
    {
        TDGameModeRef = GM;
    }
}

void USpawnWidget::OnStartWaveClicked()
{
    if (!TDGameModeRef) return;

    // Hide the widget (remove from parent; use SetVisibility if you prefer to keep it alive)
    RemoveFromParent();

    // Start wave
    TDGameModeRef->StartWave();

    // Bind to OnWaveFinished so we can show UI again
    TDGameModeRef->OnWaveFinished.AddDynamic(this, &USpawnWidget::OnWaveFinished);
}

void USpawnWidget::OnWaveFinished()
{
    // Unbind
    if (TDGameModeRef)
    {
        TDGameModeRef->OnWaveFinished.RemoveDynamic(this, &USpawnWidget::OnWaveFinished);
    }

    // Re-show the widget
    AddToViewport();
}


/*#include "SpawnWidget.h"
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
}*/

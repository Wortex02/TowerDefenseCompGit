#include "APlacementPlayerController.h"
#include "AGridManager.h"
#include "ATestTower.h"
#include "EngineUtils.h"
#include "ShopWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/World.h"

void APlacementPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("PC BeginPlay: %s"), *GetName());

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	SetShowMouseCursor(true);

	if (!PlaceableClass)
	{
		PlaceableClass = AATestTower::StaticClass();
	}
	
	if (ULocalPlayer* LP = Cast<ULocalPlayer>(Player))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (!InputMapping.IsNull())
			{
				InputSystem->AddMappingContext(InputMapping.LoadSynchronous(), 0);
			}
		}
	}
	ResolveGridManager();

	Money = StartingMoney;
	if (ShopWidgetClass)
	{
		ShopWidget = CreateWidget<UShopWidget>(this, ShopWidgetClass);
		if (ShopWidget)
		{
			ShopWidget->AddToViewport(10);
			ShopWidget->SetItems(ShopItems);
			ShopWidget->SetMoney(Money);
			ShopWidget->OnItemClicked.AddDynamic(this, &APlacementPlayerController::OnShopItemClicked);
		}
	}
}

void APlacementPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UE_LOG(LogTemp, Warning, TEXT("SetupInputComponent: %s"), *GetName());

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("EnhancedInputComponent OK"));
		if (IA_Place)
		{
			Input->BindAction(IA_Place, ETriggerEvent::Started, this, &APlacementPlayerController::OnPlaceTriggered);
			UE_LOG(LogTemp, Warning, TEXT("Bound IA_Place: %s"), *IA_Place->GetName());
		}
		else UE_LOG(LogTemp, Error, TEXT("IA_Place is NULL"));

		if (IA_P)
		{
			Input->BindAction(IA_P, ETriggerEvent::Started, this, &APlacementPlayerController::PresedP);
			UE_LOG(LogTemp, Warning, TEXT("Bound IA_P: %s"), *IA_P->GetName());
		}
		else UE_LOG(LogTemp, Error, TEXT("IA_P is NULL"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InputComponent is NOT EnhancedInputComponent"));
	}
}

void APlacementPlayerController::ResolveGridManager()
{
	if (Grid) return;

	for (TActorIterator<AGridManager> I(GetWorld()); I; ++I)
	{
		Grid = *I;
		break;
	}
}

bool APlacementPlayerController::GetMouseHitOnGrid(FVector& OutHit) const
{
	FHitResult Hit;
	if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit))
	{
		OutHit = Hit.Location;
		return true;
	}
	
	if (!Grid) return false;

	FVector WorldOrigin, WorldDir;
	if (DeprojectMousePositionToWorld(WorldOrigin, WorldDir))
	{
		if (FMath::Abs(WorldDir.Z) > KINDA_SMALL_NUMBER)
		{
			const float T = (Grid->GridZ - WorldOrigin.Z) / WorldDir.Z;
			if (T > 0.f)
			{
				OutHit = WorldOrigin + T * WorldDir;
				return true;
			}
		}
	}
	
	return false;
}

void APlacementPlayerController::OnPlaceTriggered(const FInputActionInstance&)
{
	if (!Grid) return;

	const FShopItem* Item = GetSelectedItem();
	if (!Item || !Item->TowerClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("No tower selected"));
		return;
	}
	if (!CanAffordSelected())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough money"));
		return;
	}

	FVector Hit;
	if (!GetMouseHitOnGrid(Hit))
	{
		UE_LOG(LogTemp, Warning, TEXT("GetMouseHitOnGrid(hit) failed"));
		return;
	}

	const FIntPoint Cell = Grid->WorldToCell(Hit);
	UE_LOG(LogTemp, Warning, TEXT("Placing at cell (%d,%d)"), Cell.X, Cell.Y);

	if (AActor* A = Grid->TryPlaceAtWorld(Item->TowerClass, Hit))
	{
		Money -= Item->Cost;
		UpdateUI();
		UE_LOG(LogTemp, Warning, TEXT("Spawned %s, Money now: %d"), *A->GetName(), Money);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Blocked (occupied/out of bounds)"));
	}
}

void APlacementPlayerController::OnShopItemClicked(int32 Index)
{
	if (!ShopItems.IsValidIndex(Index)) return;
	SelectedIndex = Index;
	UpdateUI();
}

const FShopItem* APlacementPlayerController::GetSelectedItem() const
{
	return ShopItems.IsValidIndex(SelectedIndex) ? &ShopItems[SelectedIndex] : nullptr;
}

bool APlacementPlayerController::CanAffordSelected() const
{
	if (const FShopItem* Item = GetSelectedItem())
	{
		return Money >= Item->Cost && Item->TowerClass != nullptr;
	}
	return false;
}

void APlacementPlayerController::UpdateUI()
{
	if (ShopWidget)
	{
		ShopWidget->SetSelectedIndex(SelectedIndex);
		ShopWidget->SetMoney(Money);
	}
}

void APlacementPlayerController::PresedP()
{
	UE_LOG(LogTemp, Warning, TEXT("Pressed P on %s"), *GetName());
}


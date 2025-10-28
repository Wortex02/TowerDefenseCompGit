#include "APlacementPlayerController.h"
#include "AGridManager.h"
#include "ATestTower.h"
#include "EngineUtils.h"

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
	if (!Grid || !PlaceableClass) return;

	FVector hit;
	if (!GetMouseHitOnGrid(hit))
	{
		UE_LOG(LogTemp, Warning, TEXT("GetMouseHitOnGrid(hit) faild"));
		return;
	}
	const FIntPoint cell = Grid->WorldToCell(hit);
	UE_LOG(LogTemp, Warning, TEXT("Placing at cell (%d,%d)"), cell.X, cell.Y);

	if (AActor* a = Grid->TryPlaceAtWorld(PlaceableClass, hit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawned %s"), *a->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Blocked (occupied/out of bounds)"));
	}
}

void APlacementPlayerController::PresedP()
{
	UE_LOG(LogTemp, Warning, TEXT("Pressed P on %s"), *GetName());
}


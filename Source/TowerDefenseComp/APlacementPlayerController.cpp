#include "APlacementPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "AGridManager.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"

void APlacementPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor       = true;
	bEnableClickEvents     = true;
	bEnableMouseOverEvents = true;
	DefaultMouseCursor     = EMouseCursor::Default;

	FInputModeGameAndUI Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetHideCursorDuringCapture(false);                         
	SetInputMode(Mode);
	
	AActor* Target = nullptr;

	if (GameCamera.IsValid())
	{
		Target = GameCamera.Get();
	}
	else
	{
		for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
		{
			Target = *It;
			break;
		}
	}

	if (Target)
	{
		SetViewTargetWithBlend(Target,0.f);
	}
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsys =
			LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC_Default)
			{
				Subsys->AddMappingContext(IMC_Default,0); 
			}
		}
	}
}
void APlacementPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Bind to the trigger state you want (Started/Triggered/Completed/etc.)
		EIC->BindAction(IA_Place, ETriggerEvent::Started, this, &APlacementPlayerController::PlaceOnClick);
	}
}

bool APlacementPlayerController::GetGroundHit(FHitResult& OutHit) const
{
	return GetHitResultUnderCursor(ECC_Visibility, false, OutHit);
}

void APlacementPlayerController::PlaceOnClick()
{
	if (!SphereClass) return;
	
	if (!GridManager.IsValid())
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), Found);
		if (Found.Num() > 0) GridManager = Cast<AGridManager>(Found[0]);
	}
	if (!GridManager.IsValid()) return;

	FHitResult Hit;
	if (!GetGroundHit(Hit)) return;

	const FVector Snapped = GridManager->SnapToGrid(Hit.ImpactPoint);
	const float  ZOffset  = 50.f;
	GetWorld()->SpawnActor<AActor>(SphereClass, Snapped + FVector(0,0,ZOffset), FRotator::ZeroRotator);
}

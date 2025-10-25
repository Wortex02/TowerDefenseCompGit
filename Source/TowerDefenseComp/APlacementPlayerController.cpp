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
#include "EnemyPoolManager.h" // your pool manager header
#include "EnemyCube.h"        // your pooled enemy pawn header
#include "Blueprint/UserWidget.h"

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

	if (CallWidgetClass)
	{
		CallWidgetInstance = CreateWidget<UUserWidget>(this, CallWidgetClass);
		if (CallWidgetInstance)
		{
			CallWidgetInstance->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APlacementPlayerController: CallWidgetClass not set in editor"));
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

void APlacementPlayerController::RequestSpawnEnemies(int32 CountOverride /*= -1*/)
{
	int32 ToSpawn = (CountOverride > 0) ? CountOverride : SpawnCount;

	// Find the pool manager in the world (there are many ways to get it; this is simple)
	AEnemyPoolManager* PoolManager = nullptr;
	for (TActorIterator<AEnemyPoolManager> It(GetWorld()); It; ++It)
	{
		PoolManager = *It;
		break;
	}

	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("RequestSpawnEnemies: No AEnemyPoolManager found in world"));
		return;
	}

	// Spawn enemies in a line by asking the pool for enemies
	for (int32 i = 0; i < ToSpawn; ++i)
	{
		AEnemyCube* E = Cast<AEnemyCube>(PoolManager->GetPooledEnemy());
		if (E)
		{
			FVector Loc = SpawnOrigin + FVector(i * SpawnSpacing, 0.f, 0.f);
			E->SetActorLocation(Loc);
			E->SetActorRotation(FRotator::ZeroRotator);

			// If your pawn needs initialization (waypoints, speed, wave id) call it here
			// e.g. E->InitializePath(SomeWaypointArray);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("RequestSpawnEnemies: Pool returned null (pool exhausted?)"));
		}
	}
}
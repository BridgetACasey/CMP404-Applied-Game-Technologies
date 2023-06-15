// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomGameMode.h"
#include "CustomARPawn.h"
#include "CustomGameState.h"
#include "CustomActor.h"
#include "CustomPlayerController.h"
#include "HelloARManager.h"
#include "ARPin.h"
#include "ARBlueprintLibrary.h"
#include "MainMenuWidget.h"
#include "HUDWidget.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "PlaceableActor.h"
#include "WhackamoleTableActor.h"

ACustomGameMode::ACustomGameMode()
{
	// Add this line to your code if you wish to use the Tick() function
	PrimaryActorTick.bCanEverTick = true;

	// Set the default pawn and gamestate to be our custom pawn and gamestate programatically
	DefaultPawnClass = ACustomARPawn::StaticClass();
	GameStateClass = ACustomGameState::StaticClass();
	PlayerControllerClass = ACustomPlayerController::StaticClass();

	//Setting up game music
	auto MenuMusic = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/Assets/Audio/Goblins_Den__Regular_.Goblins_Den__Regular_'"));
	MenuMusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MenuMusic"));
	MenuMusicComponent->SetSound(MenuMusic.Object);

	auto GameMusic = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/Assets/Audio/Goblins_Dance__Battle_.Goblins_Dance__Battle_'"));
	GameMusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("GameMusic"));
	GameMusicComponent->SetSound(GameMusic.Object);

	auto GameOverSFX = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/Assets/Audio/27_sword_miss_3.27_sword_miss_3'"));
	GameOverSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("GameOverSFX"));
	GameOverSFXComponent->SetSound(GameOverSFX.Object);

	//Ensure audio does not fire on BeginPlay
	MenuMusicComponent->SetAutoActivate(false);
	GameMusicComponent->SetAutoActivate(false);
	GameOverSFXComponent->SetAutoActivate(false);

	GameTimeCountdown = GameTimeLength;
}


void ACustomGameMode::StartPlay() 
{
	// This is called before BeginPlay
	StartPlayEvent();

	// This function will transcend to call BeginPlay on all the actors 
	Super::StartPlay();
}

// An implementation of the StartPlayEvent which can be triggered by calling StartPlayEvent() 
void ACustomGameMode::StartPlayEvent_Implementation() 
{
	auto Controller = GetWorld()->GetFirstPlayerController();

	HudWidget = CreateWidget<UHUDWidget>(Controller, HeadsUpDisplayWidget, "HUDWidget");
	MMWidget = CreateWidget<UMainMenuWidget>(Controller, MainMenuWidget, "MainMenuWidget");

	if(HudWidget)
	{
		HudWidget->OnStartMoleLevelDelegate.AddUniqueDynamic(this, &ACustomGameMode::BeginGameplay);
		HudWidget->OnEnablePauseScreenDelegate.AddUniqueDynamic(this, &ACustomGameMode::PauseGame);
		HudWidget->OnDisablePauseScreenDelegate.AddUniqueDynamic(this, &ACustomGameMode::ResumeGame);
		HudWidget->OnTriggerEndGameDelegate.AddUniqueDynamic(this, &ACustomGameMode::FinishGame);
		HudWidget->OnRestartGameDelegate.AddUniqueDynamic(this, &ACustomGameMode::RestartGame);
	}

	if (MMWidget)
	{
		MMWidget->OnMainMenuPlayDelegate.AddUniqueDynamic(this, &ACustomGameMode::BeginTablePlacement);

		MMWidget->DisplayWidget();

		if(!MenuMusicComponent->IsPlaying())
		MenuMusicComponent->FadeIn(2.0f, 1.0f, 0.0f, EAudioFaderCurve::Linear);
	}
}

void ACustomGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(HudWidget)
	{
		const auto World = GetWorld();
		const auto DerivedGameState = Cast<ACustomGameState>(World->GetGameState());

		if (StartGameTimer)
		{
			//Update UI elements in HUD widget
			HudWidget->SetDisplayScore(DerivedGameState->GetScore());
			HudWidget->SetDisplayTime(GameTimeCountdown);

			//Trigger game over state once timer runs out
			if(GameTimeCountdown < 0.0f)
				HudWidget->OnTriggerEndGameDelegate.Broadcast();

			//If allowed, decrement timer
			GameTimeCountdown -= DeltaSeconds;
		}
	}
}

void ACustomGameMode::BeginTablePlacement()
{
	//Create the AR Manager and start the session
	SpawnInitialActors();

	//Swap from the main menu to the game HUD
	if (MMWidget)
		MMWidget->HideWidget();

	if (HudWidget)
		HudWidget->DisplayWidget();
}

void ACustomGameMode::BeginGameplay()
{
	//If the table exists and is sitting on a valid piece of geometry
	if(TableActor && GeometryPlane)
	{
		TableActor->SpawnMoles(MolePlaceable, GeometryPlane);

		HasPlacedTable = true;

		const auto World = GetWorld();
		const auto DerivedGameState = Cast<ACustomGameState>(World->GetGameState());

		if (DerivedGameState)
		{
			DerivedGameState->OnUpdateGameStateDelegate.Broadcast(EGameModeState::EWhackAMole);
		}

		if (MenuMusicComponent->IsPlaying())
			MenuMusicComponent->FadeOut(2.0f, 0.0f, EAudioFaderCurve::Linear);

		if(!GameMusicComponent->IsPlaying())
			GameMusicComponent->FadeIn(2.0f, 1.0f, 0.0f, EAudioFaderCurve::Linear);

		ARManager->SetStopPlanesRendering(true);

		StartGameTimer = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, TEXT("You must place the TABLE on a VALID AR PLANE first!"));
	}
}

void ACustomGameMode::FinishGame()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, TEXT("FINISHING GAME..."));

	GameOverSFXComponent->Play();

	const auto World = GetWorld();
	const auto Pawn = Cast<ACustomARPawn>(World->GetFirstPlayerController()->GetPawn());
	const auto DerivedGameState = Cast<ACustomGameState>(World->GetGameState());

	//Update game and input states to prevent line traces and moles spawning
	if (Pawn)
		Pawn->OnUpdateInputMode(EGameModeState::EGameOver);

	if (DerivedGameState)
		DerivedGameState->OnUpdateGameStateDelegate.Broadcast(EGameModeState::EGameOver);

	if (GameMusicComponent->IsPlaying())
		GameMusicComponent->FadeOut(2.0f, 0.0f, EAudioFaderCurve::Linear);

	if (!MenuMusicComponent->IsPlaying())
		MenuMusicComponent->FadeIn(2.0f, 1.0f, 0.0f, EAudioFaderCurve::Linear);

	TableActor->SetCanMolesTrigger(false);
}

void ACustomGameMode::RestartGame()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, TEXT("RESTARTING GAME..."));

	//Check if user has pressed restart from the pause menu or game over screen
	ResumeGame();

	//Delete most existing actors
	TableActor->DestroyMoles();
	TableActor->Destroy();
	TableActor = nullptr;
	HasPlacedTable = false;
	StartGameTimer = false;
	GameTimeCountdown = GameTimeLength;

	const auto World = GetWorld();
	const auto DerivedGameState = Cast<ACustomGameState>(World->GetGameState());

	//Reset game state and relevant values
	if (DerivedGameState)
	{
		DerivedGameState->SetScore(0);
		DerivedGameState->OnUpdateGameStateDelegate.Broadcast(EGameModeState::EBuildARGeo);

		if (HudWidget)
		{
			HudWidget->SetTablePlaced(false);
			HudWidget->SetDisplayScore(0);
			HudWidget->SetDisplayTime(GameTimeLength);
		}
	}

	//Stop the AR session and reset manager to default values
	GeometryPlane = nullptr;
	ARManager->ResetARCoreSession();
	ARManager->SetStopPlanesRendering(false);
	ARManager->Reset();

	auto Pawn = Cast<ACustomARPawn>(World->GetFirstPlayerController()->GetPawn());

	if(Pawn)
	Pawn->OnUpdateInputMode(EGameModeState::EBuildARGeo);
}

void ACustomGameMode::PauseGame()
{
	if (!IsGamePaused)
	{
		IsGamePaused = true;
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		PlayerController->SetPause(true);
		//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Cyan, TEXT("PAUSING GAME"));
	}
}

void ACustomGameMode::ResumeGame()
{
	if (IsGamePaused)
	{
		IsGamePaused = false;
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		PlayerController->SetPause(false);
		//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Cyan, TEXT("RESUMING GAME"));
	}
}

void ACustomGameMode::SpawnInitialActors()
{
	// Spawn an instance of the HelloARManager class
	const FActorSpawnParameters SpawnInfo;
	const FRotator ActorRot(0, 0, 0);
	const FVector ActorLoc(0, 0, 0);

	//Create AR manager if one does not already exist
	if(ARManager == nullptr)
	ARManager = GetWorld()->SpawnActor<AHelloARManager>(ActorLoc, ActorRot, SpawnInfo);
}

void ACustomGameMode::LineTraceSpawnActor(FVector ScreenPos)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Line Trace Reached"));

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector WorldPos;
	FVector WorldDir;

	//Gets the screen touch in world space and the tracked objects from a line trace from the touch
	UGameplayStatics::DeprojectScreenToWorld(PlayerController, FVector2D(ScreenPos), WorldPos, WorldDir);

	// Notice that this LineTrace is in the ARBluePrintLibrary - this means that it's exclusive only for objects tracked by ARKit/ARCore
	auto TraceResult = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(ScreenPos), false, false, false, true);

	//Checks if the location is valid
	if (TraceResult.IsValidIndex(0))
	{
		// Get the first found object in the line trace - ignoring the rest of the array elements
		auto TrackedTF = TraceResult[0].GetLocalToWorldTransform();

		if (FVector::DotProduct(TrackedTF.GetRotation().GetUpVector(), WorldDir) < 0)
		{
			//Spawn the actor pin and get the transform
			UARPin* ActorPin = UARBlueprintLibrary::PinComponent(nullptr, TraceResult[0].GetLocalToWorldTransform(), TraceResult[0].GetTrackedGeometry());

			// Check if ARPins are available on your current device. ARPins are currently not supported locally by ARKit, so on iOS, this will always be "FALSE" 
			if (ActorPin)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, TEXT("ARPin is valid"));
				//If the pin is valid 
				auto PinTF = ActorPin->GetLocalToWorldTransform();
				//Spawn a new Actor at the location if not done yet
				if (!TableActor)
				{
					const FActorSpawnParameters SpawnInfo;
					const FRotator MyRot(0, 0, 0);
					const FVector MyLoc(ScreenPos.X, ScreenPos.Y, 0);

					if (TablePlaceable.GetDefaultObject() == nullptr)	//Checking if the subclass is valid first
					{
						GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Emerald, FString("Subclass is invalid"));
						return;
					}

					TableActor = GetWorld()->SpawnActor<AWhackamoleTableActor>(TablePlaceable, MyLoc, MyRot, SpawnInfo);
					GeometryPlane = TraceResult[0].GetTrackedGeometry();

					if (HudWidget)
						HudWidget->SetTablePlaced(true);
				}

				//If table exists, but placement hasn't been confirmed yet
				if (!HasPlacedTable)
				{
					// Set the spawned actor location based on the Pin
					TableActor->SetActorTransform(PinTF);
					TableActor->PinComponent = ActorPin;
					TableActor->PinComponent->SetPinnedComponent(TableActor->GetRootComponent());
				}
			}
		}
	}
}
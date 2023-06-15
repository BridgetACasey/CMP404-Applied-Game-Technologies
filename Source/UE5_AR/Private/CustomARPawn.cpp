// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomARPawn.h"

#include <string>

#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "ARBlueprintLibrary.h"
#include "CustomActor.h"
#include "Camera/CameraComponent.h"
#include "CustomGameMode.h"
#include "CustomGameState.h"
#include "MoleActor.h"
#include "PlaceableActor.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

//Sets default values
ACustomARPawn::ACustomARPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	
	CameraComponent->SetupAttachment(SceneComponent);
	InputMode = EInputMode::EPlaceTable;
}

//Called when the game starts or when spawned
void ACustomARPawn::BeginPlay()
{
	Super::BeginPlay();

	const auto World = GetWorld();
	const auto GameState = Cast<ACustomGameState>(World->GetGameState());

	if(!GameState)
	{
		GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, "In AR Pawn class, could not cast game state!!!");
	}
	else
	{
		GameState->OnUpdateGameStateDelegate.AddUObject(this, &ACustomARPawn::OnUpdateInputMode);
	}

}

//Called every frame
void ACustomARPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//Called to bind functionality to input
void ACustomARPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Bind player inputs to function
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACustomARPawn::OnScreenTouch);
}

bool ACustomARPawn::WorldHitTest(FVector2D ScreenPos, FHitResult& HitResult)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	//Perform deprojection, taking 2D clicked area and generating reference in 3D world space
	FVector WorldPosition;
	FVector WorldDirection;

	bool DeprojectionSuccess = UGameplayStatics::DeprojectScreenToWorld(PlayerController, ScreenPos, WorldPosition, WorldDirection);

	//Construct the trace vector (from point clicked to 1000.0 units beyond in same direction)
	FVector TraceEndVector = WorldDirection * 1000.0;
	TraceEndVector = WorldPosition + TraceEndVector;

	//Tracing one channel as we are only interested in moveable/placeable objects, so filtering by WorldDynamic
	bool TraceSuccess = GetWorld()->LineTraceSingleByChannel(HitResult, WorldPosition, TraceEndVector, ECollisionChannel::ECC_WorldDynamic);

	return TraceSuccess;
}

void ACustomARPawn::OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos)
{
	auto Temp = GetWorld()->GetAuthGameMode();
	auto GM = Cast<ACustomGameMode>(Temp);

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("ScreenTouch Reached"));

	if(GM && InputMode == EInputMode::EPlaceTable)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Attempting to spawn table"));
		GM->LineTraceSpawnActor(ScreenPos);
	}

	FHitResult HitTestResult;

	//Perform a hit test, get the return values as HitTestResult
	if (WorldHitTest(FVector2D(ScreenPos), HitTestResult))
	{
		//Get object of actor hit
		UClass* HitActorClass = UGameplayStatics::GetObjectClass(HitTestResult.GetActor());

		//Checking if we've hit a valid placeable object, otherwise ignore
		if (HitActorClass->IsChildOf(APlaceableActor::StaticClass()))
		{
			//UKismetSystemLibrary::PrintString(this, "Clicked Placeable Actor! Name: " + HitActorClass->GetName(), true, true, FLinearColor(0, 0.66, 1, 1), 2);

			AMoleActor* MolePlaceable = Cast<AMoleActor>(HitTestResult.GetActor());

			if(MolePlaceable)
			{
				const auto World = GetWorld();
				const auto GameState = Cast<ACustomGameState>(World->GetGameState());

				if (GameState)
				{
					if (GameState->GetCurrentGameState() == EGameModeState::EWhackAMole)
					{
						//UKismetSystemLibrary::PrintString(this, "CLICKED PLACEABLE MOLE: " + MolePlaceable->GetName(), true, true, FColor::Magenta, 3);
						MolePlaceable->SetSelected(true);

						//If we've hit an active mole on the table
						if (MolePlaceable->GetActiveOnTable())
						{
							//Add or remove game score based on if the mole was evil or good
							if (MolePlaceable->GetIsEvilMole())
								GameState->SetScore(GameState->GetScore() + MolePlaceable->GetScoreReward());
							else if (!MolePlaceable->GetIsEvilMole() && GameState->GetScore() > 0)
								GameState->SetScore(GameState->GetScore() - MolePlaceable->GetScoreReward());

							MolePlaceable->SetWasHitByPlayer(true);
							MolePlaceable->ResetMole();
						}
					}
				}
			}

			//Toggling the selection status of the placeable object
			APlaceableActor* Placeable = Cast<APlaceableActor>(HitActorClass);

			if (Placeable)
			{
				Placeable->ToggleSelected();

				if(Placeable->GetSelected())
					UKismetSystemLibrary::PrintString(this, "Selected: TRUE", true, true, FLinearColor(0, 0.66, 1, 1), 3);
				else
				UKismetSystemLibrary::PrintString(this, "Selected: FALSE", true, true, FLinearColor(0, 0.66, 1, 1), 3);

				//UKismetSystemLibrary::PrintString(this, Placeable->GetSelected() ? FString("Selected: TRUE") : FString("Selected: FALSE"), true, true, FLinearColor(0, 0.66, 1, 1), 3);
			}
		}
	}
}

void ACustomARPawn::OnUpdateInputMode(EGameModeState GameState)
{
	switch(GameState)
	{
	case EGameModeState::EBuildARGeo:
		InputMode = EInputMode::EPlaceTable;
		break;
	case EGameModeState::EWhackAMole:
		InputMode = EInputMode::EWhackAMole;
		break;
	case EGameModeState::EGameOver:
		InputMode = EInputMode::EGameOver;
		break;
	default:
		InputMode = EInputMode::EGameOver;
	}
}
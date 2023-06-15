// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "CustomGameMode.generated.h"

class UARTrackedGeometry;
//Forward Declarations
class APlaceableActor;
class AHelloARManager;
class UMainMenuWidget;
class UHUDWidget;
class AWhackamoleTableActor;
class AMoleActor;

/**
 * 
 */

UCLASS()
class UE5_AR_API ACustomGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ACustomGameMode();
	virtual ~ACustomGameMode() = default;

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	//For spawning a placeable actor in the scene. By default, this is the Whack-A-Mole table.
	virtual void LineTraceSpawnActor(FVector ScreenPos);

	UFUNCTION(BlueprintNativeEvent, Category = "GameModeBase", DisplayName = "Start Play")
		void StartPlayEvent();

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	virtual void BeginTablePlacement();	//To turn off the main menu widget and enter table placement mode

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	virtual void BeginGameplay();	//Enter into the main portion of the game after the table has been placed

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	virtual void FinishGame();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	virtual void RestartGame();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	virtual void PauseGame();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	virtual void ResumeGame();

	virtual void SpawnInitialActors();

	bool HasPlacedTable = false;	//Checking if the table has been placed, but the user hasn't necessarily confirmed its position
	bool StartGameTimer = false;	//To start the timer countdown in Tick
	bool IsGamePaused = false;

	AHelloARManager* ARManager = nullptr;
	UARTrackedGeometry* GeometryPlane = nullptr;

	UMainMenuWidget* MMWidget = nullptr;	//To reference the instance of the menu widget
	UHUDWidget* HudWidget = nullptr;

	UPROPERTY(Category = "Gameplay", EditAnywhere, BlueprintReadWrite)
		float GameTimeLength = 60.0f;

	UPROPERTY(Category = "Audio", EditAnywhere, BlueprintReadWrite)
		UAudioComponent* MenuMusicComponent;

	UPROPERTY(Category = "Audio", EditAnywhere, BlueprintReadWrite)
		UAudioComponent* GameMusicComponent;

	UPROPERTY(Category = "Audio", EditAnywhere, BlueprintReadWrite)
		UAudioComponent* GameOverSFXComponent;

	 UPROPERTY(Category="Placeable",EditAnywhere,BlueprintReadWrite)
	 TSubclassOf<APlaceableActor> DefaultPlaceable;

	 UPROPERTY(Category = "Placeable", EditAnywhere, BlueprintReadWrite)
		 TSubclassOf<AWhackamoleTableActor> TablePlaceable;

	 UPROPERTY(Category = "Placeable", EditAnywhere, BlueprintReadWrite)
		 TSubclassOf<AMoleActor> MolePlaceable;

	 //Using subclasses so we know which inherited blueprint to use as the final menu widget
	 UPROPERTY(Category = "UI Widgets", EditAnywhere, BlueprintReadWrite)
		 TSubclassOf<UMainMenuWidget> MainMenuWidget;

	 UPROPERTY(Category = "UI Widgets", EditAnywhere, BlueprintReadWrite)
		 TSubclassOf<UHUDWidget> HeadsUpDisplayWidget;

private:
	AWhackamoleTableActor* TableActor = nullptr;
	float GameTimeCountdown = 0.0f;	//Using a float and incrementing delta time each tick for better accuracy over an FTimer
};
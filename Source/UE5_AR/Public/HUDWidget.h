// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomUserWidget.h"
#include "HUDWidget.generated.h"

//Declaring delegates here for a straightforward way to bind C++ gameplay events to HUD elements in the blueprint widget
//Using Dynamic Multicast Delegates as we want to bind multiple functions from different classes to the same event and trigger them all with one line or button press

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartMoleLevel);	//For events triggered when the table/play area has been placed and the player has confirmed their choice

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnablePauseScreen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDisablePauseScreen);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTriggerEndGame);	//Broadcast when the timer runs out during gameplay
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRestartGame);		//Triggered from either the pause menu or the game over screen

/**
 * 
 */
UCLASS()
class UE5_AR_API UHUDWidget : public UCustomUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FOnStartMoleLevel OnStartMoleLevelDelegate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FOnEnablePauseScreen OnEnablePauseScreenDelegate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FOnDisablePauseScreen OnDisablePauseScreenDelegate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FOnTriggerEndGame OnTriggerEndGameDelegate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FOnRestartGame OnRestartGameDelegate;

	//The HUD needs to know about certain data like score and time left from CustomGameMode and CustomGameState
	//Storing them here in the C++ class so they are easily accessible from the HUDWidget blueprint

	UFUNCTION(Category="Gameplay", BlueprintCallable)
		void SetDisplayScore(int32 Score) { DisplayScore = Score; }

	UFUNCTION(Category = "Gameplay", BlueprintCallable)
		void SetDisplayTime(float Time) { DisplayTime = Time; }

	UFUNCTION(Category = "Gameplay", BlueprintCallable)
		void SetTablePlaced(bool Placed) { TablePlaced = Placed; }

protected:
	UPROPERTY(Category="Gameplay", BlueprintReadOnly)
	int32 DisplayScore = 0;

	UPROPERTY(Category = "Gameplay", BlueprintReadOnly)
	float DisplayTime = 0.0f;

	UPROPERTY(Category = "Gameplay", BlueprintReadOnly)
	bool TablePlaced = false;
};
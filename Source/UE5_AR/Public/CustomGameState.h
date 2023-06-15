// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CustomGameState.generated.h"

UENUM()
enum class EGameModeState
{
	EBuildARGeo = 0,	//For start menu and placing the table/play area
	EWhackAMole,		//For the main gameplay loop
	EGameOver			//Set when the timer runs out during the level
};

//Using event delegates with parameters for a straightforward way to change the level state from different C++ classes (or blueprints, if desired)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateGameState, EGameModeState)

/**
 * 
 */
UCLASS()
class UE5_AR_API ACustomGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	ACustomGameState();
	~ACustomGameState() = default;

	FOnUpdateGameState OnUpdateGameStateDelegate;

	UFUNCTION(Category="GameState", BlueprintGetter)
		EGameModeState GetCurrentGameState() const { return GameState; }

	UFUNCTION(Category = "Gameplay")
		void SetScore(int32 NewScore) { Score = NewScore; }

	UFUNCTION(Category = "Gameplay")
		int32 GetScore() { return Score; }

protected:
	int32 Score = 0;

private:
	EGameModeState GameState;

	void OnUpdateGameState(EGameModeState gameState);
};
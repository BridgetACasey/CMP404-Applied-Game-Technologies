// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomGameState.h"

ACustomGameState::ACustomGameState()
{
	//Binding update function to game state delegate and setting default state
	OnUpdateGameStateDelegate.AddUObject(this, &ACustomGameState::OnUpdateGameState);
	GameState = EGameModeState::EBuildARGeo;
}

void ACustomGameState::OnUpdateGameState(EGameModeState gameState)
{
	GameState = gameState;
}
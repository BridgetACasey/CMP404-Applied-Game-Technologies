// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CustomGameState.generated.h"

/**
 * 
 */
UCLASS()
class CMP404APP_API ACustomGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Score")
		int32 GetScore() const { return Score; }
	UFUNCTION(BlueprintCallable, Category = "Score")
		void SetScore(int32 NewScore) { Score = NewScore; }

private:
	int32 Score;
};
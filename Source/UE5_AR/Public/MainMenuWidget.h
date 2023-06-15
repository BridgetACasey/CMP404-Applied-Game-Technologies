// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomUserWidget.h"
#include "Components/Button.h"
#include "MainMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMainMenuPlay);	//For events triggered when transitioning from the main menu to the gameplay section
/**
 * 
 */
UCLASS()
class UE5_AR_API UMainMenuWidget : public UCustomUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable)
	FOnMainMenuPlay OnMainMenuPlayDelegate;
};
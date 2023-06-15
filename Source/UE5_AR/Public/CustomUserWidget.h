// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomUserWidget.generated.h"

//Base class for creating custom UI widgets

//Widget classes are created in C++ and then derived twice in blueprint form.
//This was necessary as the first blueprint child did not allow access to the widget designer tool.

/**
 * 
 */
UCLASS()
class UE5_AR_API UCustomUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Custom")
	void DisplayWidget(int32 index = 9999);
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category="Custom")
	void HideWidget();
};
// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomUserWidget.h"

void UCustomUserWidget::DisplayWidget(int32 index)
{
	if (!IsInViewport())
		AddToViewport(index);
}

void UCustomUserWidget::HideWidget()
{
	if (IsInViewport())
		RemoveFromViewport();
}
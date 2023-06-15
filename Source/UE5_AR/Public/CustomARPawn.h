// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "CustomARPawn.generated.h"

UENUM()
enum class EInputMode	//Enum to determine the state of user input
{
	EPlaceTable = 0,
	EWhackAMole,
	EGameOver
};

class UCameraComponent;
class AMoleActor;
enum class EGameModeState;

UCLASS()
class UE5_AR_API ACustomARPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACustomARPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//For performing line traces on valid objects
	bool WorldHitTest(FVector2D ScreenPos, FHitResult& HitResult);

	void OnUpdateInputMode(EGameModeState GameState);

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UCameraComponent* CameraComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);

	EInputMode InputMode;
};
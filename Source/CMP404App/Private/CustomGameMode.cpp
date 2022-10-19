// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameMode.h"

#include "CustomActor.h"
#include "CustomARPawn.h"
#include "CustomGameState.h"

ACustomGameMode::ACustomGameMode()
{
	DefaultPawnClass = ACustomARPawn::StaticClass();
	GameStateClass = ACustomGameState::StaticClass();
}

void ACustomGameMode::StartPlay()
{
	// This is called before any BeginPlay
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
		FString::Printf(TEXT("Current Score: %d"), GetGameState<ACustomGameState>()->GetScore()));
	// This function will transcend to call BeginPlay on all the actors
	Super::StartPlay();

	//SpawnCube();
	GetWorldTimerManager().SetTimer(Ticker, this, &ACustomGameMode::SpawnCube, 4.0f, true, 0.0f);
}

void ACustomGameMode::SpawnCube()
{
	FActorSpawnParameters SpawnInfo;
	FRotator MyRot(0, 0, 0);
	FVector MyLoc(0, 0, 0);
	ACustomActor* CustomActor = GetWorld()->SpawnActor<ACustomActor>(MyLoc, MyRot, SpawnInfo);
	UE_LOG(LogTemp, Display, TEXT("Spawning a cube!"));
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomActor.h"

#include "Components/StaticMeshComponent.h"

// Sets default values
ACustomActor::ACustomActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);
}

// Called when the game starts or when spawned
void ACustomActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACustomActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector move = GetActorLocation();

	//move = move + (1.0f * DeltaTime);
	//
	//SetActorLocation(move);

	float scale = 2.0f / (3.0f / cos(2.0f * DeltaTime));
	float x = scale * cos(DeltaTime);
	float y = scale * (sin(2.0f * DeltaTime) / 2.0f);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomARPawn.h"
#include "ARBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ACustomARPawn::ACustomARPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SceneComponent);

	//Camera information
	//APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	//FVector CamLocation = CameraManager->GetCameraLocation();
	//FVector CamForward = CameraManager->GetCameraRotation().Vector();
}

// Called when the game starts or when spawned
void ACustomARPawn::BeginPlay()
{
	Super::BeginPlay();

	//UKismetSystemLibrary::PrintString(this, FString(TEXT("Hello world")), true, true, FLinearColor::Red, 5);

	UARSessionConfig* Config = NewObject<UARSessionConfig>();
	UARBlueprintLibrary::StartARSession(Config);
}

// Called every frame
void ACustomARPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//float t = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	//float scale = 2.0f / (3.0f - cos(2.0f * t));
	//float x = 5.0f * (scale * cos(t));
	//float y = 5.0f * (scale * sin(2.0f * t) / 2.0f);
	//SetActorLocation(FVector(GetActorLocation().X + x, GetActorLocation().Y + y, GetActorLocation().Z + x));
}

// Called to bind functionality to input
void ACustomARPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomActor.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACustomActor::ACustomActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);

	// Find the Cube Asset and assign it using C++ (Right click on object in the Content Drawer to get Reference string)
	const auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);

	auto MaterialAsset = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/StarterContent/Materials/M_Wood_Floor_Walnut_Worn.M_Wood_Floor_Walnut_Worn'"));
	StaticMeshComponent->SetMaterial(0, MaterialAsset.Object);

	ActorElapsedTime = 0.0f;
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

	//FVector move = GetActorLocation();

	//move = move + (1.0f * DeltaTime);
	//
	//SetActorLocation(move);

	//float scale = 2.0f / (3.0f / cos(2.0f * DeltaTime));
	//float x = scale * cos(DeltaTime);
	//float y = scale * (sin(2.0f * DeltaTime) / 2.0f);

	//////////////////////////////////////////////////////

	float Size = 250.0f;

	//float t = UGameplayStatics::GetRealTimeSeconds(GetWorld());

	ActorElapsedTime += DeltaTime;

	float Scale = Size * (2.0f / (3.0f - cos(2.0f * ActorElapsedTime)));
	float X = Scale * cos(ActorElapsedTime);
	float Y = Scale * sin(2.0f * ActorElapsedTime) / 2.0f;

	RelativeLocation.X = X;
	RelativeLocation.Y = Y;
	RelativeLocation.Z = 0.0f;

	MatInit = FMatrix::Identity;
	MatInit.M[3][0] = InitLocation.X;
	MatInit.M[3][1] = InitLocation.Y;
	MatInit.M[3][2] = InitLocation.Z;
	
	MatMoving = FMatrix::Identity;
	MatMoving.M[3][0] = RelativeLocation.X; // this is the calculated X from the previous lab
	MatMoving.M[3][1] = RelativeLocation.Y;
	MatMoving.M[3][2] = RelativeLocation.Z;
	
	FMatrix MatFinal = MatMoving * MatInit;
	
	SetActorTransform(FTransform(MatFinal));
}

void ACustomActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	InitLocation = GetActorLocation();
}
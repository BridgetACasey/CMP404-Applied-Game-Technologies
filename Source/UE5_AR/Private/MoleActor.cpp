// Fill out your copyright notice in the Description page of Project Settings.


#include "MoleActor.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "ARPin.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AMoleActor::AMoleActor()
{
	PrimaryActorTick.bCanEverTick = true;

	//Setting the mole body scale, rotation, mesh and material
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MoleMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);

	auto BodyMeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Assets/Meshes/Andy_Mesh.Andy_Mesh'"));
	StaticMeshComponent->SetStaticMesh(BodyMeshAsset.Object);

	StaticMeshComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	StaticMeshComponent->SetRelativeScale3D(FVector(3.5f, 3.5f, 3.5f));

	//Storing pointers to the mesh instances for an easier way to swap them during runtime, as the object finder cannot be used after construction time
	GoodMoleMI = CreateDefaultSubobject<UMaterialInstance>(TEXT("GoodMoleMI"));
	EvilMoleMI = CreateDefaultSubobject<UMaterialInstance>(TEXT("EvilMoleMI"));

	//Setting location/scale and mesh and material for halo object
	AndyHaloMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AndyHaloMeshComponent"));
	AndyHaloMeshComponent->SetupAttachment(StaticMeshComponent);

	auto HaloMeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Assets/Meshes/andyhalo.andyhalo'"));
	AndyHaloMeshComponent->SetStaticMesh(HaloMeshAsset.Object);

	auto HaloMaterialAsset = ConstructorHelpers::FObjectFinder<UMaterial>(TEXT("Material'/Game/Assets/Materials/AndyHalo_Mat.AndyHalo_Mat'"));
	AndyHaloMeshComponent->SetMaterial(0, HaloMaterialAsset.Object);

	AndyHaloMeshComponent->SetRelativeLocation(AndyHaloMeshComponent->GetRelativeLocation() + FVector(0.0f, 0.0f, 20.0f));
	AndyHaloMeshComponent->SetRelativeScale3D(FVector(0.02f, 0.02f, 0.02f));


	//Setting up audio components
	auto DamageSFX = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/Assets/Audio/21_orc_damage_3.21_orc_damage_3'"));
	MoleDamageSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MoleDamageSFX"));
	MoleDamageSFXComponent->SetSound(DamageSFX.Object);

	auto HideSFX = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/Assets/Audio/22_orc_jump_2.22_orc_jump_2'"));
	MoleHideSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MoleHideSFX"));
	MoleHideSFXComponent->SetSound(HideSFX.Object);

	auto EvilActivateSFX = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/Assets/Audio/08_human_charge_1.08_human_charge_1'"));
	EvilMoleActivateSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("EvilMoleActivateSFX"));
	EvilMoleActivateSFXComponent->SetSound(EvilActivateSFX.Object);

	auto GoodActivateSFX = ConstructorHelpers::FObjectFinder<USoundWave>(TEXT("SoundWave'/Game/Assets/Audio/08_human_charge_2.08_human_charge_2'"));
	GoodMoleActivateSFXComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("GoodMoleActivateSFX"));
	GoodMoleActivateSFXComponent->SetSound(GoodActivateSFX.Object);

	MoleDamageSFXComponent->SetAutoActivate(false);
	MoleHideSFXComponent->SetAutoActivate(false);
	EvilMoleActivateSFXComponent->SetAutoActivate(false);
	GoodMoleActivateSFXComponent->SetAutoActivate(false);


	//Hiding moles until they are activated on the table
	StaticMeshComponent->SetVisibility(false);

	//If evil, give the mole a halo anyway and hide it until their morality is changed instead of creating at runtime
	if (IsEvilMole)
		AndyHaloMeshComponent->SetVisibility(false);
}

void AMoleActor::BeginPlay()
{
	Super::BeginPlay();

	AliveElapsedTime = 0.0f;
	SpawnPosition = StaticMeshComponent->GetComponentLocation();
}

void AMoleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsActiveOnTable)
	{
		AliveElapsedTime += DeltaTime;

		//Creating and scaling a translation matrix to move the mole along its local up axis, with respect to some arbitrary speed value (in this case, 4.0f)
		float OscillationValue = FMath::Sin(AliveElapsedTime);
		float AmplitudeScale = Amplitude * OscillationValue * DeltaTime * 4.0f;

		FVector Translation = StaticMeshComponent->GetUpVector() * AmplitudeScale;
		FMatrix MeshTransform(StaticMeshComponent->GetComponentTransform().ToMatrixWithScale());

		//Applying the transform to the static mesh if we're tracking
		FTranslationMatrix MeshTranslationMatrix(Translation);
		FTransform FinalMeshTransform(MeshTransform * MeshTranslationMatrix);

		//Transforming the mesh with collisions instead of the whole actor, as the actor is pinned to the tracked geometry, so cannot be moved in the same way
		switch(PinComponent->GetTrackingState())
		{
		case EARTrackingState::Tracking:
			StaticMeshComponent->SetWorldTransform(FinalMeshTransform);
			break;
		}

		//Resetting the mole automatically after a certain amount of time without the player hitting it
		if(!MoleTimer.IsValid())
			GetWorld()->GetTimerManager().SetTimer(MoleTimer, this, &AMoleActor::ResetMole, 3, false);
	}
}

void AMoleActor::PlayDeactivateSFX()
{
	if (WasHitByPlayer)
		MoleDamageSFXComponent->Play();
	else
		MoleHideSFXComponent->Play();
}

void AMoleActor::PlayActivateSFX()
{
	if (IsEvilMole)
		EvilMoleActivateSFXComponent->Play();
	else
		GoodMoleActivateSFXComponent->Play();
}

void AMoleActor::RandomMoleMoralityChance(int ChancePercent)
{
	int Chance = FMath::RandRange(0, 100);

	if (Chance < ChancePercent)
		ToggleMoleMorality();
}

void AMoleActor::ActivateMole()
{
	//Only trigger if NOT currently active
	if(!IsActiveOnTable)
	{
		//Flip morality from evil to good by random chance
		RandomMoleMoralityChance();

		//Swap materials based on morality and set appropriate meshes to visible
		if (IsEvilMole)
		{
			StaticMeshComponent->SetMaterial(0, EvilMoleMI);
			AndyHaloMeshComponent->SetVisibility(false);
		}
		else
		{
			StaticMeshComponent->SetMaterial(0, GoodMoleMI);
			AndyHaloMeshComponent->SetVisibility(true);
		}

		PlayActivateSFX();

		StaticMeshComponent->SetVisibility(true);

		IsActiveOnTable = true;
	}
}

void AMoleActor::ResetMole()
{
	//Only trigger if currently active
	if(IsActiveOnTable)
	{
		//Reset any good moles to evil so they don't stay that way for the whole game
		if(!IsEvilMole)
		{
			StaticMeshComponent->SetMaterial(0, EvilMoleMI);
			AndyHaloMeshComponent->SetVisibility(false);
			IsEvilMole = true;
		}

		StaticMeshComponent->SetWorldLocation(SpawnPosition);
		AliveElapsedTime = 0.0f;

		PlayDeactivateSFX();

		//Regardless of if they were hit by the player or timed out, reset all members to be used when mole is next activated
		StaticMeshComponent->SetVisibility(false);
		WasHitByPlayer = false;

		if (MoleTimer.IsValid())
			GetWorld()->GetTimerManager().ClearTimer(MoleTimer);

		IsActiveOnTable = false;
	}
}
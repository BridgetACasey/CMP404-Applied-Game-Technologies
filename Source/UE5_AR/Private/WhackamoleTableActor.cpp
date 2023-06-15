// Fill out your copyright notice in the Description page of Project Settings.


#include "WhackamoleTableActor.h"

#include <string>

#include "ARBlueprintLibrary.h"
#include "ARPin.h"
#include "HelloARManager.h"
#include "MoleActor.h"
#include "Engine/World.h"

AWhackamoleTableActor::AWhackamoleTableActor()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TableMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Assets/Meshes/wack-a-mole-table_Table.wack-a-mole-table_Table'"));
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);

	//Empty scene components are used to more accurately determine the correct mole positions
	//Their positions are set in the editor and these values passed to mole class in C++ when spawning moles
	for (int i = 0; i < 9; i++)
	{
		FString MoleName("MolePosition");
		MoleName.AppendInt(i);
		USceneComponent* MolePosition = CreateDefaultSubobject<USceneComponent>(*MoleName);
		MolePosition->SetupAttachment(SceneComponent);
		MoleSpawns.Add(MolePosition);
	}

	PrimaryActorTick.bCanEverTick = true;
}

void AWhackamoleTableActor::BeginPlay()
{
	Super::BeginPlay();
}

//Triggers a random amount of moles to appear on the table within a given range, assuming they're not already active
void AWhackamoleTableActor::TriggerRandomMole()
{
	int MoleCount = FMath::RandRange(1, 3);

	for (int i = 0; i < MoleCount; i++)
	{
		int Index = FMath::RandRange(0, (Moles.Num() - 1));

		AMoleActor* Mole = Moles[Index];

		Mole->ActivateMole();
	}
}

void AWhackamoleTableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(CanMolesTrigger)
	{
		//Triggering moles to appear at timed intervals
		if (!MoleTimer.IsValid())
		{
			//As multiple moles can spawn at once, they also spawn slightly faster than they auto-reset (approx. 1.5 second difference)
			//This means the level gets more difficult as time progresses, because the player must hit the moles at a quicker rate or be overwhelmed

			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Emerald, FString("Triggering mole on timer!"));
			GetWorld()->GetTimerManager().SetTimer(MoleTimer, this, &AWhackamoleTableActor::TriggerRandomMole, 1.5f, true);
		}
	}
	else
	{
		if (MoleTimer.IsValid())
			GetWorld()->GetTimerManager().ClearTimer(MoleTimer);
	}
}

void AWhackamoleTableActor::SpawnMoles(TSubclassOf<AMoleActor> MoleTemplate, UARTrackedGeometry* Geometry)
{
	for (int i = 0; i < 9; i++)
	{
		//Checking first if each USceneComponent is valid before grabbing its world position to use as a spawn point
		if (MoleSpawns[i])
		{
			FActorSpawnParameters SpawnInfo;
			FVector MoleLoc = MoleSpawns[i]->GetComponentTransform().GetLocation();
			FRotator MoleRot = MoleSpawns[i]->GetComponentTransform().Rotator();
			AMoleActor* Mole = GetWorld()->SpawnActor<AMoleActor>(MoleTemplate, MoleLoc, MoleRot, SpawnInfo);
			Mole->PinComponent = UARBlueprintLibrary::PinComponent(Mole->GetRootComponent(), MoleSpawns[i]->GetComponentTransform(), Geometry);

			//Pinning root of mole actor to the tracked geometry so its transforms update accordingly
			if (Mole->PinComponent)
			{
				Mole->SetActorTransform(Mole->PinComponent->GetLocalToWorldTransform());
				Moles.Add(Mole);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString("Mole pin component is null!"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Magenta, FString("Mole spawn position does not exist!"));
		}
	}

	//If at least some moles have spawned correctly, ensure they can be activated on the board
	if(Moles.Num() > 0)
	CanMolesTrigger = true;
}

void AWhackamoleTableActor::DestroyMoles()
{
	//Deleting existing moles, used when resetting the game
	if(Moles.Num() > 0)
	{
		for (auto Mole : Moles)
		{
			Mole->Destroy();
		}

		Moles.Empty();

		CanMolesTrigger = false;
	}
}
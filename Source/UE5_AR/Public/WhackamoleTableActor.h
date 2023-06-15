// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ARTrackable.h"
#include "PlaceableActor.h"
#include "WhackamoleTableActor.generated.h"

//The table actor is both an object and responsible for managing the moles that spawn inside of it

class AMoleActor;
/**
 * 
 */
UCLASS()
class UE5_AR_API AWhackamoleTableActor : public APlaceableActor
{
	GENERATED_BODY()
public:
	AWhackamoleTableActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SpawnMoles(TSubclassOf<AMoleActor> MoleTemplate, UARTrackedGeometry* Geometry);

	virtual void DestroyMoles();

	UFUNCTION(BlueprintCallable)
	void SetCanMolesTrigger(bool Trigger) { CanMolesTrigger = Trigger; }

	UPROPERTY(Category = "Placeable", EditAnywhere, BlueprintReadWrite)
	TArray<USceneComponent*> MoleSpawns;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void TriggerRandomMole();

private:
	TArray<AMoleActor*> Moles;	//Keeping an array of pointers to the moles for easier access later when modifying their members

	bool CanMolesTrigger = false;

	FTimerHandle MoleTimer;
};
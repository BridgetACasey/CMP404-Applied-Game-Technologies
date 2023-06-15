// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/Actor.h"
#include "PlaceableActor.generated.h"

class UARPin;

//Base actor to be used with trackable geometry that both the whack-a-mole table and moles inherit from

UCLASS()
class UE5_AR_API APlaceableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlaceableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Flag for determining if the actor has been selected by the user
	//This was intended to be used to help the user adjust a target object's transform in the scene (changing scale and rotation), but is not currently being used in the project
	bool Selected = false;

	UPROPERTY(Category = "Object", EditAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "Object", EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UARPin* PinComponent;

	void SetSelected(bool s) { Selected = s; }
	void ToggleSelected() { Selected = !Selected; }
	bool GetSelected() { return Selected; }
};

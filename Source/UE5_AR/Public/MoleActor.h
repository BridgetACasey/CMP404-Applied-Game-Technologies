// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlaceableActor.h"
#include "MoleActor.generated.h"

/**
 * 
 */
UCLASS()
class UE5_AR_API AMoleActor : public APlaceableActor
{
	GENERATED_BODY()
public:
	AMoleActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetWasHitByPlayer(bool Hit) { WasHitByPlayer = Hit; }

	bool GetActiveOnTable() const { return IsActiveOnTable; }
	bool GetWasHitByPlayer() const { return WasHitByPlayer; }
	bool GetIsEvilMole() const { return IsEvilMole; }

	//Encapsulating and exposing the actor's audio components like this as it may be played during other gameplay events
	UFUNCTION(Category = "Audio")
		void PlayDeactivateSFX();

	UFUNCTION(Category = "Audio")
		void PlayActivateSFX();

	UFUNCTION(Category = "Gameplay")
		void ActivateMole();

	UFUNCTION(Category = "Gameplay")
		void ResetMole();

	UFUNCTION(Category = "Gameplay")
		int32 GetScoreReward() { return ScoreReward; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetActiveOnTable(bool IsActive) { IsActiveOnTable = IsActive; }
	void SetIsEvilMole(bool EvilMole) { IsEvilMole = EvilMole; }

	//Randomly switch the active mole's morality from good or evil. By default, chance is 10% successful.
	UFUNCTION(Category = "Gameplay")
		void RandomMoleMoralityChance(int ChancePercent = 10);

	UFUNCTION(Category = "Gameplay")
		void ToggleMoleMorality() { IsEvilMole = !IsEvilMole; }

	//The amount added or deducted from the game score, depending on the mole's morality status
	UPROPERTY(Category = "Gameplay", EditAnywhere, BlueprintReadWrite)
		int32 ScoreReward = 50;

	//How high in units the mole should move above or below its initial spawn point
	UPROPERTY(Category = "Gameplay", EditAnywhere, BlueprintReadWrite)
		float Amplitude = 5.0f;

	UPROPERTY(Category = "Object", EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* AndyHaloMeshComponent;

	UPROPERTY(Category = "Object", EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* GoodMoleMI;

	UPROPERTY(Category = "Object", EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* EvilMoleMI;

	UPROPERTY(Category = "Audio", EditAnywhere, BlueprintReadWrite)
	UAudioComponent* MoleDamageSFXComponent;

	UPROPERTY(Category = "Audio", EditAnywhere, BlueprintReadWrite)
		UAudioComponent* MoleHideSFXComponent;

	UPROPERTY(Category = "Audio", EditAnywhere, BlueprintReadWrite)
		UAudioComponent* EvilMoleActivateSFXComponent;

	UPROPERTY(Category = "Audio", EditAnywhere, BlueprintReadWrite)
		UAudioComponent* GoodMoleActivateSFXComponent;

private:
	float AliveElapsedTime = 0.0f;	//Used to create an oscillation for the mole to move along

	bool IsActiveOnTable = false;
	bool WasHitByPlayer = false;
	bool IsEvilMole = true;	//By default, all moles start evil. Evil moles award points when hit and good moles deduct points.

	FTimerHandle MoleTimer;

	FVector SpawnPosition;	//Storing initial spawn point in case we want to lerp between here and the amplitude when mole is active
};
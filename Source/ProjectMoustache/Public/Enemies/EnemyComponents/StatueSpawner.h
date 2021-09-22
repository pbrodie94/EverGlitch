// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"

#include "StatueSpawner.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTMOUSTACHE_API UStatueSpawner : public UActorComponent
{
	GENERATED_BODY()

	//The number of statues that are currently spawned in the arena, and not destroyed
	int numberOfStatues;

	//Calculates the number of statues to spawn
	int CalculateNumberOfStatuesToSpawn(int bossPhase) const;

	//Gets a random spawn location in the volume area
	FVector GetRandomSpawnLocation();

	UBoxComponent* levelBounds;

public:	
	// Sets default values for this component's properties
	UStatueSpawner();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//Base minimum number of statues, used to calculate the number of statuses that will be spawned
	UPROPERTY(EditAnywhere)
	int baseMinNumberOfStatues;

	//Base max number of statues, used to calculate how many statues will be spawned
	UPROPERTY(EditAnywhere)
	int baseMaxNumberOfStatues;

	UPROPERTY(EditAnywhere)
	FName boundsTag;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABaseBossPowerStatue> powerStatue;
	
	//Spawns pillars when boss gets big, takes in the current phase to calculate the number of statues to spawn
	UFUNCTION(BlueprintCallable)
	void SpawnStatues(int bossPhase);

	//Called when a statue is destroyed
	UFUNCTION(BlueprintCallable)
	void OnStatueDestroyed(class ABaseBossPowerStatue* statue);

	//Used to check whether or not the boss can grow big (mostly for early development testing purposes)
	UFUNCTION(BlueprintCallable)
	bool HasVolumeReference();
		
};

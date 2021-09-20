// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyComponents/StatueSpawner.h"

// Sets default values for this component's properties
UStatueSpawner::UStatueSpawner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	baseMinNumberOfStatues = 3;
	baseMinNumberOfStatues = 5;
}


// Called when the game starts
void UStatueSpawner::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if (baseMinNumberOfStatues <= 0)
	{
		baseMinNumberOfStatues = 3;
	}
	
	if (baseMaxNumberOfStatues <= 0)
	{
		baseMaxNumberOfStatues = 5;
	}
}


// Called every frame
void UStatueSpawner::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UStatueSpawner::SpawnStatues(uint8 bossPhase)
{
	uint8 statuesToSpawn = CalculateNumberOfStatuesToSpawn(bossPhase);

	for (int i = 0; i < statuesToSpawn; ++i)
	{
		//Get position and rotation for each pillar
		FVector position = GetRandomSpawnLocation();
		FRotator rotation = FRotator(0, FMath::RandRange(0, 360), 0);

		++numberOfStatues;
	}
}

/**
 * Takes in the current boss phase and then gets a random number of statues to spawn
 */
uint8 UStatueSpawner::CalculateNumberOfStatuesToSpawn(uint8 bossPhase) const
{
	short min = (--bossPhase) + baseMinNumberOfStatues;
	short max = (--bossPhase) + baseMaxNumberOfStatues;

	uint8 numberOfStatues = FMath::RandRange(min, max);
	
	return numberOfStatues;
}


/**
 * Gets a random location within the spawn volume, then returns it
 */
FVector UStatueSpawner::GetRandomSpawnLocation()
{
	//Get random position within the spawn volume

	//Calculate the ground position

	return FVector::ZeroVector;
}


/**
 * Called when a statue has been destroyed.
 * Once the number of statues has reached 0, boss will shrink to natural size
 */
void UStatueSpawner::OnPillarDestroyed()
{
	if (numberOfStatues <= 0)
		return;

	//Decrement number of statues
	--numberOfStatues;

	//Check if all statues have been destroyed
	if (numberOfStatues <= 0)
	{
		//Trigger boss to shrink to normal size
	}
}


/**
 * Checks if the component has a reference to the volume of area that is used to get random positions to spawn
 * statues in.
 * This check is to see if the boss can perform his growing action, only on a return true.
 */
bool UStatueSpawner::HasVolumeReference()
{
	return false;
}



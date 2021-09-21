// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/BaseBossPowerStatue.h"
#include "Enemies/EnemyComponents/StatueSpawner.h"

#include <string>

#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UStatueSpawner::UStatueSpawner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	baseMinNumberOfStatues = 3;
	baseMaxNumberOfStatues = 5;
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

	//Find the bounding area for the boss arena
	for (TActorIterator<AActor> actorIterator(GetWorld()); actorIterator; ++actorIterator)
	{
		AActor* actor = *actorIterator;
		if (actor && actor != GetOwner())
		{
			for (FName tag : actor->Tags)
			{
				if (tag == boundsTag)
				{
					levelBounds = actor->FindComponentByClass<UBoxComponent>();

					break;
				}
			}
		}
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

		//int statuesLeft = statuesToSpawn - i;
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Statues left " + FString::FromInt(statuesLeft) + " Spawn position: " + position.ToString()));

		//Spawn the statue
		FActorSpawnParameters SpawnParameters;
		ABaseBossPowerStatue* statue = GetWorld()->SpawnActor<ABaseBossPowerStatue>(
			powerStatue,
			position,
			rotation,
			SpawnParameters);

		//Bind the on destroyed function to the statue's delegate function
		statue->OnStatueDestroyed.BindUObject(this, &UStatueSpawner::OnStatueDestroyed);
		
		++numberOfStatues;
	}
}

/**
 * Takes in the current boss phase and then gets a random number of statues to spawn
 */
uint32 UStatueSpawner::CalculateNumberOfStatuesToSpawn(uint32 bossPhase) const
{
	short min = (--bossPhase) + baseMinNumberOfStatues;
	short max = (--bossPhase) + baseMaxNumberOfStatues;

	uint32 statuesToSpawn = FMath::RandRange(min, max);
	
	return statuesToSpawn;
}


/**
 * Gets a random location within the spawn volume, then returns it
 */
FVector UStatueSpawner::GetRandomSpawnLocation()
{
	//Get random position within the spawn volume
	FVector position = UKismetMathLibrary::RandomPointInBoundingBox(
		levelBounds->GetOwner()->GetActorLocation(), levelBounds->GetScaledBoxExtent());

	//Calculate the ground position
	float height = levelBounds->GetScaledBoxExtent().Z;
	FVector start = position;
	FVector end = start + FVector(0, 0, -height);
	FHitResult hitResult;
	FCollisionQueryParams collisionParams(FName("GroundCheck"), false, GetOwner());
	if (GetWorld()->LineTraceSingleByChannel(
		hitResult, start, end, ECC_Visibility, collisionParams))
	{
		position.Z = hitResult.ImpactPoint.Z;
	}

	return position;
}


/**
 * Called when a statue has been destroyed.
 * Once the number of statues has reached 0, boss will shrink to natural size
 */
void UStatueSpawner::OnStatueDestroyed(ABaseBossPowerStatue* statue)
{
	statue->OnStatueDestroyed.Unbind();
	
	if (numberOfStatues <= 0)
		return;

	//Decrement number of statues
	--numberOfStatues;

	//Check if all statues have been destroyed
	if (numberOfStatues <= 0)
	{
		//Trigger boss to shrink to normal size
	}

	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, TEXT("Statue destroyed, " + FString::FromInt(numberOfStatues) + " statues left"));
}


/**
 * Checks if the component has a reference to the volume of area that is used to get random positions to spawn
 * statues in.
 * This check is to see if the boss can perform his growing action, only on a return true.
 */
bool UStatueSpawner::HasVolumeReference()
{
	return levelBounds != nullptr;
}



// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/EnemyComponents/StatueSpawner.h"
#include "Enemies/BaseBossPowerStatue.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "Enemies/BaseBoss.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UStatueSpawner::UStatueSpawner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	levelBounds = nullptr;
	hasLevelReference = false;

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
					if (levelBounds != nullptr)
					{
						hasLevelReference = true;
					}
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

void UStatueSpawner::SpawnStatues(int bossPhase)
{
	int statuesToSpawn = CalculateNumberOfStatuesToSpawn(bossPhase);
	
	for (int i = 0; i < statuesToSpawn; ++i)
	{
		//Get position and rotation for each pillar
		FVector position = GetRandomSpawnLocation();
		FRotator rotation = FRotator(0, FMath::RandRange(0, 360), 0);

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
int UStatueSpawner::CalculateNumberOfStatuesToSpawn(int bossPhase) const
{
	short min = (--bossPhase) + baseMinNumberOfStatues;
	short max = (--bossPhase) + baseMaxNumberOfStatues;

	int statuesToSpawn = FMath::RandRange(min, max);
	
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
	float height = levelBounds->GetScaledBoxExtent().Z + 1000;
	FVector start = position;
	FVector end = start + (FVector::DownVector * height);
	FHitResult hitResult;
	FCollisionQueryParams collisionParams(FName("GroundCheck"), false, GetOwner());
	collisionParams.AddIgnoredActor(levelBounds->GetOwner());
	if (GetWorld()->LineTraceSingleByChannel(
		hitResult, start, end, ECC_Visibility, collisionParams))
	{
		position = hitResult.ImpactPoint;
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
		ABaseBoss* boss = Cast<ABaseBoss>(GetOwner());
		if (boss != nullptr)
		{
			boss->BeginShrinkSize();
		}
	}
}


/**
 * Checks if the component has a reference to the volume of area that is used to get random positions to spawn
 * statues in.
 * This check is to see if the boss can perform his growing action, only on a return true.
 */
bool UStatueSpawner::HasVolumeReference()
{
	return hasLevelReference;
}

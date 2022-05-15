// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemySpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemies/EnemyBase.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	spawnZone = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnZone"));
	spawnZone->SetBoxExtent(FVector(500.0f, 500.0f, 500.0f));
	spawnZone->SetCollisionResponseToAllChannels(ECR_Ignore);
	spawnZone->SetupAttachment(RootComponent);

	spawnOverlapCheckBounds = FVector(100.0f, 100.0f, 100.0f);
	numOverlapCheckIterations = 25;

	groundTag = "Ground";
	
	ignoreActors.Add(this);
	
	queryObjects.Add(UEngineTypes::ConvertToObjectType(ECC_Camera));
	queryObjects.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	spawnOnBeginPlay = true;
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	numOverlapCheckIterations = FMath::Clamp(numOverlapCheckIterations, 5, 50);

	if (spawnOverlapCheckBounds.X < 1)
	{
		spawnOverlapCheckBounds.X = 1.0f;
	}

	if (spawnOverlapCheckBounds.Y < 1)
	{
		spawnOverlapCheckBounds.Y = 1.0f;
	}

	if (spawnOverlapCheckBounds.Z < 1)
	{
		spawnOverlapCheckBounds.Z = 1.0f;
	}

	InitializeEnemyList();
	
	defaultNumberOfEnemies = FMath::Clamp(defaultNumberOfEnemies, 0, 20);

	if (spawnOnBeginPlay)
	{
		SpawnEnemies();
	}
}

/**
* Tries to spawn a default amount of enemies
*/
void AEnemySpawner::SpawnEnemies()
{
	SpawnNumberOfEnemies(defaultNumberOfEnemies);
}

/**
* Tries to spawn a passed in number of enemies
*/
void AEnemySpawner::SpawnNumberOfEnemies(int numberOfEnemies)
{
	if (spawnEnemies.Num() <= 0 || numberOfEnemies <= 0)
	{
		return;
	}

	for (int i = 0; i < numberOfEnemies; ++i)
	{
		// Tries to get a spawn location, stops spawning if no free location is found
		const FVector spawnLocation = GetRandomLocationInBounds();
		if (spawnLocation == FVector::ZeroVector)
		{
			return;
		}

		// Tries to get an enemy to spawn, stops spawning if null pointer is returned
		const TSubclassOf<AEnemyBase> spawnEnemy = GetEnemyToSpawn();
		if (spawnEnemy == nullptr)
		{
			return;
		}

		// Spawns enemy at the location with a random yaw rotation
		const FRotator randomRotation = FRotator(0, FMath::RandRange(0.0f, 360.0f), 0);
		GetWorld()->SpawnActor<AEnemyBase>(spawnEnemy, spawnLocation, randomRotation);
	}
}

/**
 * Gets a random enemy using importance
 * Enemies are sorted in the list by importance with the highest importance being at the front
 * A random tier is selected from the available tiers, then a random index is chosen from the
 * available indexes.
 * A higher index can only select higher importance indexes, but lower tiers have a random chance
 * of selecting a higher tier index as well.
 */
TSubclassOf<AEnemyBase> AEnemySpawner::GetEnemyToSpawn() const
{
	if (spawnEnemies.Num() <= 0)
	{
		return nullptr;
	}

	if (spawnEnemies.Num() == 1)
	{
		return spawnEnemies[0].enemyObject;
	}

	// First get a random tier
	const int tier = FMath::RandRange(0, importanceTiers.Num() - 1);
	// Then get a random index from the available indexes
	const int index = FMath::RandRange(0, importanceTiers[tier]);

	return spawnEnemies[index].enemyObject;
}

/**
* Returns a random location within the bounds of the spawnZone box volume
* Checks for overlaps using the spawn overlap check bounds
* Returns ZeroVector if no non obstructed location could be found
*/
FVector AEnemySpawner::GetRandomLocationInBounds() const
{
	FVector spawnLocation = FVector::ZeroVector;

	int checkIteration = 0;

	// Iterate getting random location, until free from obstructions
	do
	{
		// Infinite loop guard
		if (checkIteration >= numOverlapCheckIterations)
		{
			return FVector::ZeroVector;
		}

		// Get random point in spawnZone
		spawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), spawnZone->GetScaledBoxExtent());

		// Put point on the ground
		float height = spawnZone->GetScaledBoxExtent().Z * 2;
		FVector start = spawnLocation;
		FVector end = start + (FVector::DownVector * height);
		FHitResult hitResult;
		FCollisionQueryParams collisionParams(FName(groundTag), false, this);
		if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility, collisionParams))
		{
			spawnLocation = hitResult.ImpactPoint + FVector(0.0f, 0.0f, 50.0f);
		}

		++checkIteration;
		
	} while (CheckIsPointOverlapping(spawnLocation));

	return spawnLocation;
}

/**
* Overlap check for choosing random points
* Uses spawnOverlapCheckBounds for the dimensions of the check
*/
bool AEnemySpawner::CheckIsPointOverlapping(FVector checkLocation) const
{
	// Box trace at location using overlap bounds size
	TArray<FHitResult> hitResults;
	const bool boxTraceHit = UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetWorld(),
		checkLocation,
		checkLocation,
		spawnOverlapCheckBounds,
		FRotator(0.0f, 0.0f, 0.0f),
		queryObjects,
		false,
		ignoreActors,
		EDrawDebugTrace::None,
		hitResults,
		true);

	// If trace hits object that is not the ground, then the location is obstructed
	if (boxTraceHit)
	{
		for (const FHitResult result : hitResults)
		{
			if (result.Actor == nullptr)
			{
				continue;
			}
			
			if (result.Actor->ActorHasTag(groundTag))
			{
				continue;
			}
			
			return true;
		}
	}

	return false;
}

/**
* Sorts and sets the tier indexes
*/
void AEnemySpawner::InitializeEnemyList()
{
	if (spawnEnemies.Num() <= 0)
	{
		return;
	}

	// Sort enemies by importance, with highest in front
	SortSpawnEnemies();

	int tier = 1;
	for (int i = 0; i < spawnEnemies.Num(); ++i)
	{
		// Checks if the tier of the enemy is different from the current tier
		if (spawnEnemies[i].spawnPriority != tier)
		{
			/**
			 * Adds blank tiers in case there is a split between importance
			 * e.g. enemy importance of 1 and of 3. Importance 2 will pick from
			 * importance 3 and 2, but sice importance 2 is empty, it will only pick from 3
			 */
			if (spawnEnemies[i].spawnPriority > (tier + 1))
			{
				// Foreach tier between the last and current, add the highest index as last enemy
				const int difference = (spawnEnemies[i].spawnPriority - tier) + 1;
				for (int j = 0; j < difference; ++j)
				{
					importanceTiers.Add(i - 1);
				}
			}

			// Adds last index of the tier
			importanceTiers.Add(i);
			// Sets current tier for rest of check
			tier = spawnEnemies[i].spawnPriority;
		}
	}

	if (importanceTiers.Num() <= 0)
	{
		importanceTiers.Add(spawnEnemies.Num() - 1);
	}
}

/**
* Sorts enemies in enemy list by priority
* Uses bubble sort method
* Sorts higher priority to the front of the list 
*/
void AEnemySpawner::SortSpawnEnemies()
{
	// If 1 or less enemies, skip
	if (spawnEnemies.Num() <= 1)
	{
		return;
	}

	// Bubble sort
	for (int i = spawnEnemies.Num() - 1; i >= 0; --i)
	{
		for (int j = 1; j <= i; ++j)
		{
			if (spawnEnemies[j - 1].spawnPriority < spawnEnemies[j].spawnPriority)
			{
				const FSpawnEnemy temp = spawnEnemies[j];
				spawnEnemies[j] = spawnEnemies[j - 1];
				spawnEnemies[j - 1] = temp; 
			}
		}
	}
}


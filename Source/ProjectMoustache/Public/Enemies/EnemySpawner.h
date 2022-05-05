// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

USTRUCT(BlueprintType)
struct FSpawnEnemy
{
	GENERATED_BODY()

	// Enemy object to be spawned 
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AEnemyBase> enemyObject;

	// Higher priority means higher chance of being spawned
	UPROPERTY(EditAnywhere)
	int spawnPriority = 1;

	FSpawnEnemy()
	{
		if (spawnPriority <= 0)
		{
			spawnPriority = 1;
		}
	}
};

UCLASS()
class PROJECTMOUSTACHE_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

	// Holds indexes of the different tiers of importance of enemy
	TArray<unsigned int> importanceTiers;

	/**
	 * Sorts and sets the tier indexes
	 */
	void InitializeEnemyList();

	/**
	 * Returns a random location within the bounds of the spawnZone box volume
	 * Checks for overlaps using the spawn overlap check bounds
	 */
	FVector GetRandomLocationInBounds() const;

	/**
	 * Overlap check for choosing random points
	 * Uses spawnOverlapCheckBounds for the dimensions of the check
	 */
	bool CheckIsPointOverlapping(FVector checkLocation) const;

	/**
	 * Returns a random enemy to spawn
	 * accounting for spawn priorities
	 */
	TSubclassOf<class AEnemyBase> GetEnemyToSpawn() const;

	/**
	 * Sorts enemies in enemy list by priority
	 * Uses bubble sort method
	 * Sorts higher priority to the front of the list 
	 */
	void SortSpawnEnemies();

protected:

	// The actor tag used to identify the ground
	UPROPERTY(EditDefaultsOnly)
	FName groundTag;

	// Bounds where spawner will try to get a random spawn location
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBoxComponent* spawnZone;

	// Dimensions used to check for overlaps when choosing locations to spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector spawnOverlapCheckBounds;

	// Number of times overlap check will iterate before returning failed
	UPROPERTY(EditDefaultsOnly)
	int numOverlapCheckIterations;

	// Objects to query for
	UPROPERTY(EditDefaultsOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> queryObjects;

	// Objects to ignore
	UPROPERTY(EditDefaultsOnly)
	TArray<AActor*> ignoreActors;

	// Enemies this zone will spawn with their priorities
	UPROPERTY(EditAnywhere)
	TArray<FSpawnEnemy> spawnEnemies;

	// The number of enemies zone will spawn by default clamped between 0 - 20
	UPROPERTY(EditAnywhere)
	int defaultNumberOfEnemies;

	// Sets if zone will spawn enemies on begin play
	UPROPERTY(EditAnywhere)
	bool spawnOnBeginPlay;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	
	// Sets default values for this actor's properties
	AEnemySpawner();

	/**
	 * Tries to spawn a default amount of enemies
	 */
	UFUNCTION(BlueprintCallable)
	void SpawnEnemies();

	/**
	 * Tries to spawn a passed in number of enemies
	 */
	UFUNCTION(BlueprintCallable)
	void SpawnNumberOfEnemies(int numberOfEnemies);
};

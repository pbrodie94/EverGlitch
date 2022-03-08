// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CombatManagerComponent.h"
#include "Enemies/EnemyBase.h"

// Sets default values for this component's properties
UCombatManagerComponent::UCombatManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	maxAttackingEnemies = 3;
	maxEngagingEnemies = 6;
}


// Called when the game starts
void UCombatManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (maxEngagingEnemies <= 0)
	{
		maxEngagingEnemies = 6;
	}

	if (maxAttackingEnemies <= 0)
	{
		maxAttackingEnemies = 0;
	}
}


// Called every frame
void UCombatManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

/**
* Allows enemies to request to engage player
* Engaging enemies are allowed to get into battle range
* Returns true if there is space for an engaging enemy
* Otherwise returns false
*/
bool UCombatManagerComponent::RequestEngagePlayer(AEnemyBase* enemyRef)
{
	// If null, return false 
	if (enemyRef == nullptr)
	{
		return false;
	}
	
	// Return true if already engaging
	if (engagingEnemies.Contains(enemyRef))
	{
		return true;
	}

	// If space for engaging enemy, add and return true
	if (engagingEnemies.Num() < maxEngagingEnemies)
	{
		engagingEnemies.AddUnique(enemyRef);
		return true;
	}

	return false;
}

/**
* Returns true if passed in enemy is registered to be engaging
*/
bool UCombatManagerComponent::GetIsEngaging(AEnemyBase* enemyRef)
{
	// If null return false
	if (enemyRef == nullptr)
	{
		return false;
	}
	
	if (!engagingEnemies.Contains(enemyRef))
	{
		return false;
	}

	return true;
}

/**
* Notifies finished engaging player
*/
void UCombatManagerComponent::NotifyFinishedEngaging(AEnemyBase* enemyRef)
{
	// If engaging enemies list does not contain the passed in enemy, do nothing
	if (!engagingEnemies.Contains(enemyRef))
	{
		return;
	}

	engagingEnemies.Remove(enemyRef);	
}

/**
* Allows enemies to request to attack
* Attacking enemies are able to perform an attack
* This is to ensure no more than a set number of enemies are performing an attack at any one time
*/
bool UCombatManagerComponent::RequestAttackPlayer(AEnemyBase* enemyRef)
{
	// If null or too many attackers, return false
	if (enemyRef == nullptr || attackingEnemies.Num() >= maxAttackingEnemies)
	{
		return false;
	}

	// Enemy is already attacking, return true
	if (attackingEnemies.Contains(enemyRef))
	{
		return true;
	}

	attackingEnemies.AddUnique(enemyRef);

	return true;
}

/**
* Returns true if passed in enemy is registered to be attacking
*/
bool UCombatManagerComponent::GetIsAttacking(AEnemyBase* enemyRef)
{
	if (enemyRef == nullptr)
	{
		return false;
	}

	if (!attackingEnemies.Contains(enemyRef))
	{
		return false;
	}

	return true;
}

/**
* Enemy has finished attacking
*/
void UCombatManagerComponent::NotifyFinishedAttacking(AEnemyBase* enemyRef)
{
	if (!attackingEnemies.Contains(enemyRef))
	{
		return;
	}

	attackingEnemies.Remove(enemyRef);
}

/**
* Subscribes enemies to the combat manager
*/
void UCombatManagerComponent::SubscribeSelfToCombatManager(AEnemyBase* enemyRef)
{
	if (activeEnemies.Contains(enemyRef))
	{
		return;
	}

	activeEnemies.AddUnique(enemyRef);

	// Subscribe to delegates that remove enemy from list if they are no longer alerted attacking player
	enemyRef->OnDied.AddUniqueDynamic(this, &UCombatManagerComponent::UnsubscribeFromCombatManager);
}

/**
* Removes an enemy from the combat manager
*/
void UCombatManagerComponent::UnsubscribeFromCombatManager(AEnemyBase* enemyRef)
{
	if (enemyRef == nullptr || !activeEnemies.Contains(enemyRef))
	{
		return;
	}

	activeEnemies.Remove(enemyRef);

	if (engagingEnemies.Contains(enemyRef))
	{
		engagingEnemies.Remove(enemyRef);
	}

	if (attackingEnemies.Contains(enemyRef))
	{
		attackingEnemies.Remove(enemyRef);
	}
}


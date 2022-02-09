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
* Returns true if there is space for an engaging enemy
* Otherwise returns false
*/
bool UCombatManagerComponent::RequestEngagePlayer(TScriptInterface<IEnemyInterface> enemyRef)
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
* Notifies finished engaging player
*/
void UCombatManagerComponent::NotifyFinishedEngaging(TScriptInterface<IEnemyInterface> enemyRef)
{
	if (!engagingEnemies.Contains(enemyRef))
	{
		return;
	}

	engagingEnemies.Remove(enemyRef);	
}

/**
* Allows enemies to request to attack
*/
bool UCombatManagerComponent::RequestAttackPlayer(TScriptInterface<IEnemyInterface> enemyRef)
{
	if (enemyRef == nullptr || attackingEnemies.Num() >= maxAttackingEnemies)
	{
		return false;
	}

	if (attackingEnemies.Contains(enemyRef))
	{
		return true;
	}

	attackingEnemies.AddUnique(enemyRef);

	return true;
}

/**
* Enemy has finished attacking
*/
void UCombatManagerComponent::NotifyFinishedAttacking(TScriptInterface<IEnemyInterface> enemyRef)
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
void UCombatManagerComponent::SubscribeSelfToCombatManager(TScriptInterface<IEnemyInterface> enemyRef)
{
	if (activeEnemies.Contains(enemyRef))
	{
		return;
	}

	activeEnemies.AddUnique(enemyRef);

	AEnemyBase* enemy = Cast<AEnemyBase>(enemyRef.GetObject());
	if (enemy != nullptr)
	{
		enemy->OnDied.AddDynamic(this, &UCombatManagerComponent::UnsubscribeFromCombatManager);
	}
}

/**
* Removes an enemy from the combat manager
*/
void UCombatManagerComponent::UnsubscribeFromCombatManager(TScriptInterface<IEnemyInterface> enemyRef)
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
}


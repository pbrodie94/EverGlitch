// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enemies/EnemyInterface.h"
#include "CombatManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTMOUSTACHE_API UCombatManagerComponent : public UActorComponent
{
	GENERATED_BODY()

	// Maximum amount of enemies that can be attacking at a time
	UPROPERTY(EditDefaultsOnly)
	int maxAttackingEnemies;

	// Maximum number of enemies that can be engaging at a time
	UPROPERTY(EditDefaultsOnly)
	int maxEngagingEnemies;
	
	// Active enemies
	UPROPERTY()
	TArray<TScriptInterface<IEnemyInterface>> activeEnemies;

	// Engaging enemies
	UPROPERTY()
	TArray<TScriptInterface<IEnemyInterface>> engagingEnemies;

	// Attacking enemies
	UPROPERTY()
	TArray<TScriptInterface<IEnemyInterface>> attackingEnemies;

public:	
	// Sets default values for this component's properties
	UCombatManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Allows enemies to request to engage player
	 * Returns true if there is space for an engaging enemy
	 * Otherwise returns false
	 */
	UFUNCTION(BlueprintCallable)
	bool RequestEngagePlayer(TScriptInterface<IEnemyInterface> enemyRef);

	/**
	 * Notifies finished engaging player
	 */
	UFUNCTION(BlueprintCallable)
	void NotifyFinishedEngaging(TScriptInterface<IEnemyInterface> enemyRef);

	/**
	 * Allows enemies to request to attack
	 */
	UFUNCTION(BlueprintCallable)
	bool RequestAttackPlayer(TScriptInterface<IEnemyInterface> enemyRef);

	/**
	 * Enemy has finished attacking
	 */
	UFUNCTION(BlueprintCallable)
	void NotifyFinishedAttacking(TScriptInterface<IEnemyInterface> enemyRef);
	
	/**
	 * Subscribes enemies to the combat manager
	 */
	UFUNCTION(BlueprintCallable)
	void SubscribeSelfToCombatManager(TScriptInterface<IEnemyInterface> enemyRef);

	/**
	 * Removes an enemy from the combat manager
	 */
	UFUNCTION(BlueprintCallable)
	void UnsubscribeFromCombatManager(TScriptInterface<IEnemyInterface> enemyRef);
		
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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
	TArray<class AEnemyBase*> activeEnemies;

	// Engaging enemies
	UPROPERTY()
	TArray<class AEnemyBase*> engagingEnemies;

	// Attacking enemies
	UPROPERTY()
	TArray<class AEnemyBase*> attackingEnemies;

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
	 * Engaging enemies are allowed to get into battle range
	 * Returns true if there is space for an engaging enemy
	 * Otherwise returns false
	 */
	UFUNCTION(BlueprintCallable)
	bool RequestEngagePlayer(class AEnemyBase* enemyRef);

	/**
	* Returns true if passed in enemy is registered to be engaging
	*/
	UFUNCTION(BlueprintCallable)
	bool GetIsEngaging(class AEnemyBase* enemyRef);

	/**
	 * Notifies finished engaging player
	 */
	UFUNCTION(BlueprintCallable)
	void NotifyFinishedEngaging(class AEnemyBase* enemyRef);

	/**
	 * Allows enemies to request to attack
	 * Attacking enemies are able to perform an attack
	 * This is to ensure no more than a set number of enemies are performing an attack at any one time
	 */
	UFUNCTION(BlueprintCallable)
	bool RequestAttackPlayer(class AEnemyBase* enemyRef);

	/**
	 * Returns true if passed in enemy is registered to be attacking
	 */
	UFUNCTION(BlueprintCallable)
	bool GetIsAttacking(class AEnemyBase* enemyRef);

	/**
	 * Enemy has finished attacking
	 */
	UFUNCTION(BlueprintCallable)
	void NotifyFinishedAttacking(class AEnemyBase* enemyRef);
	
	/**
	 * Subscribes enemies to the combat manager
	 */
	UFUNCTION(BlueprintCallable)
	void SubscribeSelfToCombatManager(class AEnemyBase* enemyRef);

	/**
	 * Removes an enemy from the combat manager
	 */
	UFUNCTION(BlueprintCallable)
	void UnsubscribeFromCombatManager(class AEntityBase* enemyRef);
		
};

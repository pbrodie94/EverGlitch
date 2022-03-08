// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyInterface.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatManagerResponses, class AEnemyBase*, enemyInterface);

UCLASS()
class PROJECTMOUSTACHE_API AEnemyBase : public ACharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyBase();

protected:

	UPROPERTY(BlueprintReadWrite)
	float moveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float walkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float runSpeed;

	// Health the enemy starts with, defaults to max health
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float startingHealth;

	// Maximum possible health, if less than 1: defaults to 100
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float maxHealth;

	// Current health
	UPROPERTY(BlueprintReadWrite)
	float currentHealth;

	// Base attack damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float baseDamage;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Die();
	void Die_Implementation();

public:

	// Delegate for enemy dying
	UPROPERTY(BlueprintAssignable)
	FCombatManagerResponses OnDied;

	// Delegate for enemy transitioning from alerted state
	UPROPERTY(BlueprintAssignable)
	FCombatManagerResponses OnSwitchedState;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetMoveSpeed(bool running);
	
	/**
	* Returns the enemy's current health
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetCurrentHealth();
	float GetCurrentHealth_Implementation() { return currentHealth; }

	/**
	* Returns the enemy's maximum health
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetMaxHealth();
	float GetMaxHealth_Implementation() { return maxHealth; }

	/**
	* Returns if the enemy is still alive
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetIsDead();
	bool GetIsDead_Implementation() { return currentHealth <= 0; }
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EntityBase.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatManagerResponses, class AEnemyBase*, enemyInterface);

UCLASS()
class PROJECTMOUSTACHE_API AEnemyBase : public AEntityBase
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

	// Base attack damage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
	float baseDamage;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Die_Implementation() override;

public:

	// Delegate for enemy dying
	UPROPERTY(BlueprintAssignable)
	FCombatManagerResponses OnDied;

	// Delegate for enemy transitioning from alerted state
	UPROPERTY(BlueprintAssignable)
	FCombatManagerResponses OnSwitchedState;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetMoveSpeed(bool running);
};

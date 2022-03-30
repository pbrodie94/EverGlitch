// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Damageable.h"
#include "StatusEffect.generated.h"

/*************************************************************
* Status Effect
*************************************************************/
UCLASS()
class PROJECTMOUSTACHE_API UStatusEffectBase : public UObject
{
	GENERATED_BODY()

	bool isExpired;

protected:
	
	UPROPERTY()
	AActor* effectedActor;

	UPROPERTY()
	AActor* damageCauser;

	UPROPERTY()
	AController* eventInstigator;

	float effectAmount;
	
	float duration;
	float timeEnded;
	
public:

	UStatusEffectBase();

	virtual void Init(AActor* actor, float amount, float effectDuration, float interval);

	virtual void UpdateStatus();

	FORCEINLINE float GetTimeRemaining() const;

	FORCEINLINE void SetIsExpired(bool expired) { isExpired = expired; }

	virtual FORCEINLINE EStatusEffectType GetEffectType() { return None; }

	FORCEINLINE float GetEffectAmount() const { return effectAmount; }

	FORCEINLINE bool GetIsExpired() const { return isExpired; }
};

/*************************************************************
* Burn Effect
*************************************************************/
UCLASS()
class PROJECTMOUSTACHE_API UBurnStatus : public UStatusEffectBase
{
	GENERATED_BODY()
	
	float damageInterval;

	float timeNextDamage;

public:

	UBurnStatus();

	virtual void Init(AActor* actor, float amount, float effectDuration, float interval) override;
	
	virtual void UpdateStatus() override;

	virtual FORCEINLINE EStatusEffectType GetEffectType() override { return Burn; }
};

/************************************************************
* Chilled Effect
************************************************************/

UCLASS()
class UChilledStatus : public UStatusEffectBase
{
	GENERATED_BODY()
public:

	virtual FORCEINLINE EStatusEffectType GetEffectType() override { return Chilled; }
};

/************************************************************
* Wet Effect
************************************************************/

UCLASS()
class UWetStatus : public UStatusEffectBase
{
	GENERATED_BODY()
public:

	virtual void Init(AActor* actor, float amount, float effectDuration, float interval) override;

	virtual FORCEINLINE EStatusEffectType GetEffectType() override { return Wet; }
};

/************************************************************
* Static Effect
************************************************************/

UCLASS()
class UStaticEffect : public UStatusEffectBase
{
	GENERATED_BODY()

public:

	virtual FORCEINLINE EStatusEffectType GetEffectType() override { return Static; }
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Damageable.h"
#include "StatusEffect.generated.h"

/************************************************************
* Status Effect
*************************************************************/
UCLASS()
class PROJECTMOUSTACHE_API UStatusEffectBase : public UObject
{
	GENERATED_BODY()

protected:
	
	UPROPERTY()
	AActor* effectedActor;

	UPROPERTY()
	AActor* damageCauser;

	UPROPERTY()
	AController* eventInstigator;
	
	float duration;
	float timeEnded;
	
public:

	UStatusEffectBase();

	virtual void Init(AActor* actor, float effectDuration, float worldTime);

	virtual void UpdateStatus();

	virtual FORCEINLINE EStatusEffectType GetEffectType() { return None; }
	
};

/************************************************************
* Burn Effect
*************************************************************/
UCLASS()
class PROJECTMOUSTACHE_API UBurnStatus : public UStatusEffectBase
{
	GENERATED_BODY()

	float damageAmount;
	float damageInterval;

	float timeNextDamage;

public:

	UBurnStatus();

	virtual void Init(AActor* actor, float effectDuration, float worldTime) override;
	
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

/***********************************************************
* Wet Effect
************************************************************/

UCLASS()
class UWetStatus : public UStatusEffectBase
{
	GENERATED_BODY()
public:

	virtual FORCEINLINE EStatusEffectType GetEffectType() override { return Wet; }
};

/***********************************************************
* Static Effect
************************************************************/

UCLASS()
class UStaticEffect : public UStatusEffectBase
{
	GENERATED_BODY()
public:

	virtual FORCEINLINE EStatusEffectType GetEffectType() override { return Static; }
};
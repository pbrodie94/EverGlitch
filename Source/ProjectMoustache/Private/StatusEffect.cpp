// Fill out your copyright notice in the Description page of Project Settings.

#include "Damageable.h"
#include "StatusEffect.h"

/*************************************************************************************************************
 * Status Effect
 ************************************************************************************************************/
UStatusEffectBase::UStatusEffectBase()
{
	
}

void UStatusEffectBase::Init(AActor* actor, float effectDuration, float worldTime)
{
	effectedActor = actor;
	duration = effectDuration;
	timeEnded = worldTime + duration;
}

void UStatusEffectBase::UpdateStatus()
{	
	if (GetWorld()->GetTimeSeconds() > timeEnded)
	{
		IDamageable::Execute_RemoveStatusEffect(effectedActor, this);
	}
}

/*************************************************************************************************************
* Burn Effect
*************************************************************************************************************/
UBurnStatus::UBurnStatus()
{
	
}

void UBurnStatus::Init(AActor* actor, float effectDuration, float worldTime)
{
	UStatusEffectBase::Init(actor, effectDuration, worldTime);

	timeNextDamage = worldTime + damageInterval;
}

void UBurnStatus::UpdateStatus()
{
	UStatusEffectBase::UpdateStatus();

	float worldTime = GetWorld()->GetTimeSeconds();
	
	// Deal damage on tick interval
	if (worldTime > timeNextDamage)
	{
		const FStatusEffect effect(Burn);
		IDamageable::Execute_TakeIncomingDamage(effectedActor, damageAmount, damageCauser, eventInstigator, FDamageData(effect));
		
		timeNextDamage = worldTime + damageInterval;
	}
}

/*************************************************************************************************************
* Chilled Effect
*************************************************************************************************************/

/*************************************************************************************************************
* Wet Effect
*************************************************************************************************************/

/*************************************************************************************************************
* Static Effect
*************************************************************************************************************/
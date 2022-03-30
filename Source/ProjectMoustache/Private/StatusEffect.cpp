// Fill out your copyright notice in the Description page of Project Settings.

#include "StatusEffect.h"
#include "Damageable.h"

/*************************************************************************************************************
 * Status Effect
 ************************************************************************************************************/
UStatusEffectBase::UStatusEffectBase()
{
	isExpired = false;
}

void UStatusEffectBase::Init(AActor* actor, float amount, float effectDuration, float interval)
{
	effectedActor = actor;
	duration = effectDuration;
	timeEnded = GetWorld()->GetTimeSeconds() + duration;
	effectAmount = amount;
}

void UStatusEffectBase::UpdateStatus()
{	
	if (GetWorld()->GetTimeSeconds() > timeEnded)
	{
		isExpired = true;
		IDamageable::Execute_RemoveStatusEffect(effectedActor, this);
	}
}

float UStatusEffectBase::GetTimeRemaining() const
{
	return timeEnded - GetWorld()->GetTimeSeconds();
}

/*************************************************************************************************************
* Burn Effect
*************************************************************************************************************/
UBurnStatus::UBurnStatus()
{
	
}

void UBurnStatus::Init(AActor* actor, float amount, float effectDuration, float interval)
{
	Super::Init(actor, amount, effectDuration, interval);
	damageInterval = interval > 0 ? interval : 1;
	
	timeNextDamage = GetWorld()->GetTimeSeconds() + damageInterval;
}

void UBurnStatus::UpdateStatus()
{
	Super::UpdateStatus();

	const float worldTime = GetWorld()->GetTimeSeconds();
	
	// Deal damage on tick interval
	if (worldTime > timeNextDamage)
	{
		const FStatusEffect effect(Burn);
		IDamageable::Execute_TakeIncomingDamage(effectedActor, effectAmount, damageCauser, eventInstigator, FDamageData(effect));
		
		timeNextDamage = worldTime + damageInterval;
	}
}

/*************************************************************************************************************
* Chilled Effect
*************************************************************************************************************/

/*************************************************************************************************************
* Wet Effect
*************************************************************************************************************/

void UWetStatus::Init(AActor* actor, float amount, float effectDuration, float interval)
{
	const float effectPercentage = FMath::Clamp(amount, 0.0f, 100.0f);
	Super::Init(actor, effectPercentage, effectDuration, interval);
}

/*************************************************************************************************************
* Static Effect
*************************************************************************************************************/
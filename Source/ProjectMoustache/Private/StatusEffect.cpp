// Fill out your copyright notice in the Description page of Project Settings.

#include "StatusEffect.h"
#include "Damageable.h"

/*************************************************************************************************************
 * Status Effect
 ************************************************************************************************************/
UStatusEffectBase::UStatusEffectBase()
{
	duration = 0;
	timeEnded = 0;
	effectAmount = 0;
	effectedActor = nullptr;
	damageCauser = nullptr;
	eventInstigator = nullptr;
	isExpired = false;
}

void UStatusEffectBase::Init(AActor* actor, float amount, float effectDuration, float interval, float worldTime)
{
	effectedActor = actor;
	duration = effectDuration;
	timeEnded = worldTime + duration;
	effectAmount = amount;

	timeRemaining = duration;
}

void UStatusEffectBase::UpdateStatus(float worldTime)
{
	if (worldTime > timeEnded)
	{
		SetExpired();
		IDamageable::Execute_RemoveStatusEffect(effectedActor, this);
	}

	timeRemaining = timeEnded - worldTime;
}

void UStatusEffectBase::SetExpired()
{
	OnExpired();
	isExpired = true;
}

/*************************************************************************************************************
* Burn Effect
*************************************************************************************************************/
UBurnStatus::UBurnStatus()
{

}

void UBurnStatus::Init(AActor* actor, float amount, float effectDuration, float interval, float worldTime)
{
	Super::Init(actor, amount, effectDuration, interval, worldTime);
	damageInterval = interval > 0 ? interval : 1;

	timeNextDamage = worldTime + damageInterval;
}

void UBurnStatus::UpdateStatus(float worldTime)
{
	Super::UpdateStatus(worldTime);

	// Deal damage on tick interval
	if (worldTime > timeNextDamage)
	{
		const FStatusEffect effect(Burn);
		IDamageable::Execute_TakeIncomingDamage(effectedActor, effectAmount, nullptr, eventInstigator, FDamageData(effect));

		timeNextDamage = worldTime + damageInterval;
	}
}

/*************************************************************************************************************
* Chilled Effect
*************************************************************************************************************/

void UChilledStatus::Init(AActor* actor, float amount, float effectDuration, float interval, float worldTime)
{
	Super::Init(actor, amount, effectDuration, interval, worldTime);

	IDamageable* damagableActor = Cast<IDamageable>(effectedActor);
	if(damagableActor == nullptr)
	{
		return;
	}

	defaultSpeed = damagableActor->GetMoveSpeed();
	effectedSpeed = defaultSpeed / 2;
	damagableActor->SetMoveSpeed(defaultSpeed / 2);
}

void UChilledStatus::OnExpired()
{
	Super::OnExpired();

	IDamageable* damagableActor = Cast<IDamageable>(effectedActor);
	if(damagableActor == nullptr)
	{
		return;
	}

	damagableActor->SetMoveSpeed(defaultSpeed);
}

/*************************************************************************************************************
* Wet Effect
*************************************************************************************************************/

void UWetStatus::Init(AActor* actor, float amount, float effectDuration, float interval, float worldTime)
{
	const float effectPercentage = FMath::Clamp(amount, 0.0f, 100.0f);
	Super::Init(actor, effectPercentage, effectDuration, interval, worldTime);
}

/*************************************************************************************************************
* Static Effect
*************************************************************************************************************/

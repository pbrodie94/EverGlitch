// Fill out your copyright notice in the Description page of Project Settings.


#include "EntityBase.h"

#include "StatusEffect.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEntityBase::AEntityBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	maxHealth = 100;
	startingHealth = 100;
	currentHealth = 100;

	fireEffectiveness = 100;
	iceEffectiveness = 100;
	lightningEffectiveness = 100;
	waterEffectiveness = 100;
}

// Called when the game starts or when spawned
void AEntityBase::BeginPlay()
{
	Super::BeginPlay();

	if (maxHealth <= 0)
	{
		maxHealth = 100;
	}

	currentHealth = startingHealth > 0 ? startingHealth : maxHealth;

	fireEffectiveness = FMath::Clamp(fireEffectiveness, -100.0f, 200.0f);
	iceEffectiveness = FMath::Clamp(iceEffectiveness, -100.0f, 200.0f);
	lightningEffectiveness = FMath::Clamp(lightningEffectiveness, -100.0f, 200.0f);
	waterEffectiveness = FMath::Clamp(waterEffectiveness, -100.0f, 200.0f);
}

// Called every frame
void AEntityBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Remove old status effects
	if (removedStatusEffects.Num() > 0)
	{
		for (UStatusEffectBase* removedEffect : removedStatusEffects)
		{
			statusEffects.Remove(removedEffect);
			OnStatusEffectRemoved.Broadcast(removedEffect);
		}

		removedStatusEffects.Empty();
	}
	
	// Add new status effects
	if (newStatusEffects.Num() > 0)
	{
		for (UStatusEffectBase* newEffect : newStatusEffects)
		{
			statusEffects.Add(newEffect);
			OnStatusEffectAdded.Broadcast(newEffect);
		}

		newStatusEffects.Empty();
	}
	
	// Update current status effects
	if (statusEffects.Num() > 0)
	{
		for (UStatusEffectBase* statusEffect : statusEffects)
		{
			statusEffect->UpdateStatus(GetWorld()->GetTimeSeconds());
		}
	}
}

/*********************************************************************
* IDamageable
*********************************************************************/
float AEntityBase::TakeIncomingDamage_Implementation(float damageAmount, AActor* damageCauser, AController* eventInstigator, FDamageData damageData)
{
	if (GetIsDead())
	{
		return 0;
	}
	
	if (damageAmount <= 0 || currentHealth <= 0)
	{
		return 0;
	}

	float damage = damageAmount;

	// Adjust damage based on status effects
	if (damageData.statusEffect.statusEffectType != None)
	{
		const FStatusEffect elementEffect = damageData.statusEffect;
		switch (elementEffect.statusEffectType)
		{
		case Burn:
			damage *= (fireEffectiveness / 100);
			break;
		case Chilled:
			damage *= (iceEffectiveness / 100);
			break;
		case Wet:
			damage *= (waterEffectiveness / 100);
			break;
		case Static:
			damage *= (lightningEffectiveness / 100);

			// Add damage increase if wet
			if (GetHasStatusEffect(Wet))
			{
				UWetStatus* wetStatus = Cast<UWetStatus>(GetStatusEffect(Wet));
				if (wetStatus != nullptr)
				{
					damage *= (1 + (wetStatus->GetEffectAmount() / 100));
				}
			}
			
			break;
		default:
			break;
		}

		// Determine if status effect is applied
		if (elementEffect.statusInflictChance > 0 && elementEffect.duration > 0)
		{
			const float rand = FMath::RandRange(0, 100);
			if (rand < elementEffect.statusInflictChance)
			{
				AddStatusEffect(elementEffect);
			}
		}
	}

	currentHealth -= damage;
	if (currentHealth <= 0)
	{
		currentHealth = 0;
		Die();
	}
	
	return damage;
}

/**
* Takes in a status effect struct and adds it to the character
* If the resistance to the element is <= 0, the effect will not be added
*/
void AEntityBase::AddStatusEffect_Implementation(FStatusEffect statusEffect)
{
	UStatusEffectBase* status = nullptr;
	
	switch (statusEffect.statusEffectType)
	{
		case Burn:

			if (GetHasStatusEffect(Wet))
			{
				RemoveStatus(Wet);
				return;
			}
			
			if (fireEffectiveness <= 0)
			{
				return;
			}

			// Check if already have burn status
			if (GetHasStatusEffect(Burn))
			{
				// Check if burn status has less time left than new status
				UStatusEffectBase* burnStatus = GetStatusEffect(Burn);
				if (burnStatus != nullptr)
				{
					if (burnStatus->GetTimeRemaining() > statusEffect.duration)
					{
						return;
					}
				}

				// Remove then add new status
				RemoveStatus(Burn);
			}
			
			status = NewObject<UBurnStatus>();
		
			break;
		case Chilled:

			if (iceEffectiveness <= 0)
			{
				return;
			}

			// Check if has chilled status
			if (GetHasStatusEffect(Chilled))
			{
				UStatusEffectBase* chilledStatus = GetStatusEffect(Chilled);
				if (chilledStatus != nullptr)
				{
					if (chilledStatus->GetTimeRemaining() > statusEffect.duration)
					{
						return;
					}
				}

				RemoveStatus(Chilled);
			}

			status = NewObject<UChilledStatus>();
			
			break;
		case Static:

			if (lightningEffectiveness <= 0)
			{
				return;
			}

			// Check if has static status
			if (GetHasStatusEffect(Static))
			{
				UStatusEffectBase* staticStatus = GetStatusEffect(Static);
				if (staticStatus != nullptr)
				{
					if (staticStatus->GetTimeRemaining() > statusEffect.duration)
					{
						return;
					}
				}

				RemoveStatus(Static);
			}

			status = NewObject<UStaticEffect>();
			
			break;
		case Wet:

			if (waterEffectiveness <= 0)
			{
				return;
			}

			// Check if has wet status
			if (GetHasStatusEffect(Wet))
			{
				UStatusEffectBase* wetStatus = GetStatusEffect(Wet);
				if (wetStatus != nullptr)
				{
					if (wetStatus->GetTimeRemaining() > statusEffect.duration)
					{
						return;
					}

					RemoveStatus(Wet);
				}
			}
		
			status = NewObject<UWetStatus>();
			
			break;
		case Frozen:
			break;
		default:
			break;
	}

	if (status == nullptr)
	{
		return;
	}
	
	status->Init(this, statusEffect.effectAmount, statusEffect.duration, statusEffect.dotInterval, GetWorld()->GetTimeSeconds());
	newStatusEffects.AddUnique(status);
}

float AEntityBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (GetIsDead())
	{
		return 0;
	}
	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageAmount <= 0 || currentHealth <= 0)
	{
		return 0;
	}

	currentHealth -= DamageAmount;
	if (currentHealth <= 0)
	{
		currentHealth = 0;
		Die();
	}

	return DamageAmount;
}

void AEntityBase::SetMoveSpeed(float speed)
{
	GetCharacterMovement()->MaxWalkSpeed = speed;	
}

float AEntityBase::GetMoveSpeed() const
{
	return GetCharacterMovement()->MaxWalkSpeed;	
}

void AEntityBase::Die_Implementation()
{
	currentHealth = 0;
	
	RemoveAllStatusEffects();
	
	if (OnDied.IsBound())
	{
		OnDied.Broadcast(this);
	}
}

/**
* Removes all status effects of a type
*/
void AEntityBase::RemoveStatus_Implementation(EStatusEffectType statusEffect)
{
	if (statusEffects.Num() <= 0)
	{
		return;
	}

	for (UStatusEffectBase* status : statusEffects)
	{
		if (status->GetEffectType() == statusEffect)
		{
			status->SetExpired();
			removedStatusEffects.AddUnique(status);
		}
	}
}

/**
* Called by a status effect to remove themselves
*/
void AEntityBase::RemoveStatusEffect_Implementation(UStatusEffectBase* statusEffect)
{
	if (statusEffects.Num() <= 0)
	{
		return;
	}

	statusEffect->SetExpired();
	removedStatusEffects.Add(statusEffect);
}

void AEntityBase::RemoveAllStatusEffects()
{
	if (statusEffects.Num() <= 0)
	{
		return;
	}

	for (UStatusEffectBase* statusEffect : statusEffects)
	{
		statusEffect->SetExpired();
		removedStatusEffects.AddUnique(statusEffect);
	}

	newStatusEffects.Empty();
}

/**
* Returns a list of status effects active on the character
*/
TArray<UStatusEffectBase*> AEntityBase::GetAllStatusEffects_Implementation()
{
	TArray<UStatusEffectBase*> effects;

	for (UStatusEffectBase* status : statusEffects)
	{
		effects.AddUnique(status);
	}

	return effects;
}

/**
* Returns the first found non-expired object for the inputted status effect type
* If the status effect does not exist, it will return nullptr
*/
UStatusEffectBase* AEntityBase::GetStatusEffect(EStatusEffectType effectType)
{
	if (statusEffects.Num() <= 0)
	{
		return nullptr;
	}

	for (UStatusEffectBase* status : statusEffects)
	{
		if (status->GetEffectType() == effectType && !status->GetIsExpired())
		{
			return status;
		}
	}

	return nullptr;
}

/**
* Returns true if the character has the status effect
*/
bool AEntityBase::GetHasStatusEffect_Implementation(EStatusEffectType statusEffect)
{
	for (UStatusEffectBase* status : statusEffects)
	{
		if (status->GetEffectType() == statusEffect && !status->GetIsExpired())
		{
			return true;
		}
	}
	
	return false;
}

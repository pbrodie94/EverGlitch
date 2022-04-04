// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyBase.h"

#include "StatusEffect.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	maxHealth = 100;

	fireResistance = 0;
	iceResistance = 0;
	lightningResistance = 0;
	waterResistance = 0;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (maxHealth <= 1)
	{
		maxHealth = 100;
	}

	currentHealth = startingHealth > 0 ? startingHealth : maxHealth;

	fireResistance = FMath::Clamp(fireResistance, -100.0f, 200.0f);
	iceResistance = FMath::Clamp(iceResistance, -100.0f, 200.0f);
	lightningResistance = FMath::Clamp(lightningResistance, -100.0f, 200.0f);
	waterResistance = FMath::Clamp(waterResistance, -100.0f, 200.0f);
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Remove old status effects
	if (removedStatusEffects.Num() > 0)
	{
		for (UStatusEffectBase* removedEffect : removedStatusEffects)
		{
			statusEffects.Remove(removedEffect);
		}

		removedStatusEffects.Empty();
	}
	
	// Add new status effects
	if (newStatusEffects.Num() > 0)
	{
		for (UStatusEffectBase* newEffect : newStatusEffects)
		{
			statusEffects.Add(newEffect);
		}

		newStatusEffects.Empty();
	}
	
	// Update current status effects
	if (statusEffects.Num() > 0)
	{
		for (UStatusEffectBase* statusEffect : statusEffects)
		{
			statusEffect->UpdateStatus();
		}
	}
}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageAmount <= 0 || currentHealth <= 0)
	{
		return 0;
	}

	currentHealth -= DamageAmount;
	if (currentHealth <= 0)
	{
		currentHealth = 0;
		OnDied.Broadcast(this); 
		Die();
	}
	
	return DamageAmount;
}

void AEnemyBase::Die_Implementation()
{
	OnDied.Broadcast(this);
}

void AEnemyBase::SetMoveSpeed(bool running)
{
	GetCharacterMovement()->MaxWalkSpeed = running ? runSpeed : walkSpeed;
}

/*********************************************************************
* IDamageable
*********************************************************************/

float AEnemyBase::TakeIncomingDamage_Implementation(float damageAmount, AActor* damageCauser, AController* eventInstigator, FDamageData damageData)
{
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
				damage *= (fireResistance / 100);
				break;
			case Chilled:
				damage *= (iceResistance / 100);
				break;
			case Wet:
				damage *= (waterResistance / 100);
				break;
			case Static:
				damage *= (lightningResistance / 100);

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
		OnDied.Broadcast(this); 
		Die();
	}
	
	return damage;
}

/**
* Takes in a status effect struct and adds it to the character
* If the resistance to the element is <= 0, the effect will not be added
*/
void AEnemyBase::AddStatusEffect_Implementation(FStatusEffect statusEffect)
{
	/*UStatusEffectBase* status = nullptr;
	
	switch (statusEffect.statusEffectType)
	{
		case Burn:

			if (GetHasStatusEffect(Wet))
			{
				RemoveStatus(Wet);
				return;
			}
			
			if (fireResistance <= 0)
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

			if (iceResistance <= 0)
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

			if (lightningResistance <= 0)
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

			if (waterResistance <= 0)
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
	
	status->Init(this, statusEffect.effectAmount, statusEffect.duration, statusEffect.dotInterval);
	newStatusEffects.Add(status);*/
}

/**
* Removes all status effects of a type
*/
void AEnemyBase::RemoveStatus_Implementation(EStatusEffectType statusEffect)
{
	if (statusEffects.Num() <= 0)
	{
		return;
	}

	for (UStatusEffectBase* status : statusEffects)
	{
		if (status->GetEffectType() == statusEffect)
		{
			status->SetIsExpired(true);
			removedStatusEffects.Add(status);
		}
	}
}

/**
* Called by a status effect to remove themselves
*/
void AEnemyBase::RemoveStatusEffect_Implementation(UStatusEffectBase* statusEffect)
{
	if (statusEffects.Num() <= 0)
	{
		return;
	}

	statusEffect->SetIsExpired(true);
	removedStatusEffects.Add(statusEffect);
}

/**
* Returns the first found non-expired object for the inputted status effect type
* If the status effect does not exist, it will return nullptr
*/
UStatusEffectBase* AEnemyBase::GetStatusEffect(EStatusEffectType effectType)
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
bool AEnemyBase::GetHasStatusEffect_Implementation(EStatusEffectType statusEffect)
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


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

	fireResistance = FMath::Clamp(fireResistance, -100.0f, 100.0f);
	iceResistance = FMath::Clamp(iceResistance, -100.0f, 100.0f);
	lightningResistance = FMath::Clamp(lightningResistance, -100.0f, 100.0f);
	waterResistance = FMath::Clamp(waterResistance, -100.0f, 100.0f);
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
				damage *= (lightningResistance / 100);\
				break;
			default:
				break;
		}

		if (elementEffect.statusInflictChance > 0 && elementEffect.duration > 0)
		{
			float rand = FMath::RandRange(0, 100);
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
	UStatusEffectBase* status = nullptr;
	
	switch (statusEffect.statusEffectType)
	{
		case Burn:
			
			if (fireResistance <= 0)
			{
				return;
			}
			
			status = NewObject<UBurnStatus>();
		
			break;
		case Chilled:

			if (iceResistance <= 0)
			{
				return;
			}

			status = NewObject<UChilledStatus>();
			
			break;
		case Static:

			if (lightningResistance <= 0)
			{
				return;
			}

			status = NewObject<UStaticEffect>();
			
			break;
		case Wet:

			if (waterResistance <= 0)
			{
				return;
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
	
	status->Init(this, statusEffect.duration, GetWorld()->GetTimeSeconds());
	newStatusEffects.Add(status);
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

	removedStatusEffects.Add(statusEffect);
}

/**
* Returns true if the character has the status effect
*/
bool AEnemyBase::GetHasStatusEffect_Implementation(EStatusEffectType statusEffect)
{
	for (UStatusEffectBase* status : statusEffects)
	{
		if (status->GetEffectType() == statusEffect)
		{
			return true;
		}
	}
	
	return false;
}


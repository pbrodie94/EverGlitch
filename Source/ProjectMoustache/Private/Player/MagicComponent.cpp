// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MagicComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Damageable.h"
#include "Player/PlayerCharacter.h"

// Sets default values for this component's properties
UMagicComponent::UMagicComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
}

// Called when the game starts
void UMagicComponent::BeginPlay()
{
	Super::BeginPlay();

	// Set default spells if they are set
	if (startingDestructionSpell != nullptr)
	{
		SetDestructionMagicSpell(startingDestructionSpell);
		currentSpell = destructionSpell;
	}

	if (startingSupportSpell != nullptr)
	{
		SetSupportMagicSpell(startingSupportSpell);
		if (currentSpell == nullptr)
		{
			currentSpell = supportSpell;
		}
	}
}

void UMagicComponent::CastDestructionSpell()
{
	if (isCasting)
	{
		return;
	}
	
	if (GetOwner()->Implements<UPlayerCharacter>())
	{
		if (!IPlayerCharacter::Execute_GetHasControl(GetOwner()))
		{
			return;
		}
	}
	
	if (!GetOwner()->Implements<UDamageable>())
	{
		return;
	}

	if (IDamageable::Execute_GetIsDead(GetOwner()))
	{
		return;
	}
	
	if (destructionSpell != nullptr)
	{
		APawn* ownerPawn = Cast<APawn>(GetOwner());
		if (destructionSpell->Execute(ownerPawn))
		{
			currentSpell = destructionSpell;
			isCasting = true;
		}		
	}
}

void UMagicComponent::CastSupportSpell()
{
	if (isCasting)
	{
		return;
	}
	
	if (GetOwner()->Implements<UPlayerCharacter>())
	{
		if (!IPlayerCharacter::Execute_GetHasControl(GetOwner()))
		{
			return;
		}
	}
	
	if (!GetOwner()->Implements<UDamageable>())
	{
		return;
	}

	if (IDamageable::Execute_GetIsDead(GetOwner()))
	{
		return;
	}
	
	if (supportSpell != nullptr)
	{
		APawn* ownerPawn = Cast<APawn>(GetOwner());
		if (supportSpell->Execute(ownerPawn))
		{
			currentSpell = supportSpell;
			isCasting = true;
		}		
	}
}

void UMagicComponent::CastMagicProjectile()
{
	if (!isCasting || currentSpell == nullptr)
	{
		return;
	}

	APawn* ownerPawn = Cast<APawn>(GetOwner());
	currentSpell->CastSpell(ownerPawn);

	isCasting = false;
}

void UMagicComponent::CancelCasting()
{
	isCasting = false;	
}

/**
* Creates and Equips a new destruction spell
*/
void UMagicComponent::SetDestructionMagicSpell(TSubclassOf<UMagicSpellBase> newMagicSpell)
{
	if (newMagicSpell == nullptr)
	{
		return;
	}

	// Destroy old spell
	if (destructionSpell != nullptr)
	{
		destructionSpell->ConditionalBeginDestroy();
		destructionSpell = nullptr;
	}

	// Create and equip new spell
	if (newMagicSpell != nullptr)
	{
		UMagicSpellBase* spell = NewObject<UMagicSpellBase>(GetOwner(), newMagicSpell);
		destructionSpell = spell;
	}
}

/**
* Creates and Equips a new support spell
*/
void UMagicComponent::SetSupportMagicSpell(TSubclassOf<UMagicSpellBase> newSupportSpell)
{
	if (newSupportSpell == nullptr)
	{
		return;
	}

	// Destroy old spell
	if (supportSpell != nullptr)
	{
		supportSpell->ConditionalBeginDestroy();
		supportSpell = nullptr;
	}

	// Create and equip new support spell
	if (newSupportSpell != nullptr)
	{
		UMagicSpellBase* spell = NewObject<UMagicSpellBase>(GetOwner(), newSupportSpell);
		supportSpell = spell;
	}
}

UMagicSpellBase* UMagicComponent::GetCurrentDestructionSpell()
{
	return destructionSpell;
}

UMagicSpellBase* UMagicComponent::GetCurrentSupportSpell()
{
	return supportSpell;
}



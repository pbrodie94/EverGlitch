// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MagicComponent.h"

#include <lmerr.h>

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UMagicComponent::UMagicComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMagicComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UInputComponent* inputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (inputComponent != nullptr)
	{
		inputComponent->BindAction("Ability1", IE_Pressed, this, &UMagicComponent::CastSupportSpell);
		inputComponent->BindAction("Ability2", IE_Pressed, this, &UMagicComponent::CastPowerSpell);
	}
	
}


// Called every frame
void UMagicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMagicComponent::CastPowerSpell()
{
	if (powerSpell != nullptr)
	{
		APawn* ownerPawn = Cast<APawn>(GetOwner());
		powerSpell->Execute(ownerPawn);
	}
}

void UMagicComponent::CastSupportSpell()
{
	if (supportSpell != nullptr)
	{
		APawn* ownerPawn = Cast<APawn>(GetOwner());
		supportSpell->Execute(ownerPawn);
	}
}

void UMagicComponent::SetPowerMagicSpell_Implementation(AMagicSpellBase* newMagicSpell)
{
	if (newMagicSpell != nullptr)
	{
		powerSpell = newMagicSpell;
	}
}

void UMagicComponent::SetSupportMagicSpell_Implementation(AMagicSpellBase* newSupportSpell)
{
	if (newSupportSpell != nullptr)
	{
		supportSpell = newSupportSpell;
	}
}

AMagicSpellBase* UMagicComponent::GetCurrentPowerSpell()
{
	return powerSpell;
}

AMagicSpellBase* UMagicComponent::GetCurrentSupportSpell()
{
	return supportSpell;
}



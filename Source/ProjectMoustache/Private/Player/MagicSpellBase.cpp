// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MagicSpellBase.h"

#include "Kismet/KismetMathLibrary.h"

UMagicSpellBase::UMagicSpellBase()
{
}

void UMagicSpellBase::Execute(APawn* userActor)
{
	if (spellProjectile == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
			TEXT("Error: Spell projectile not set!"));
		return;
	}
	
	UWorld* world = GetWorld();
	if (world->GetTimeSeconds() < timeNextFire)
	{
		return;
	}

	// TODO: Add call to get spawn point from player character when created
	const FRotator spawnRotation = userActor->GetControlRotation();
	const FVector spawnPosition = userActor->GetActorLocation() +
		(UKismetMathLibrary::GetForwardVector(spawnRotation) * 150.0f);
	const FVector spawnScale = FVector(1.0f, 1.0f, 1.0f);

	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = userActor;
	spawnParameters.Instigator = userActor;

	world->SpawnActor<AActor>(spellProjectile, FTransform(spawnRotation, spawnPosition, spawnScale),
		spawnParameters);

	timeFired = world->GetTimeSeconds();
	timeNextFire = world->GetTimeSeconds() + coolDownTime;
}

float UMagicSpellBase::GetCooldownProgress()
{
	const float time = (GetWorld()->GetTimeSeconds() - timeFired) / coolDownTime;
	return FMath::Clamp(time, 0.0f, 1.0f);
}

UWorld* UMagicSpellBase::GetWorld() const 
{
	if (GIsEditor && !GIsPlayInEditorWorld)
	{
		return nullptr;
	}
	else if (GetOuter())
	{
		return GetOuter()->GetWorld();
	}
	else
	{
		return nullptr;
	}
}

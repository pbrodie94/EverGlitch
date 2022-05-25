// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MagicSpellBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/PlayerCharacter.h"
#include "Weapons/WeaponBase.h"

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
	FRotator spawnRotation = userActor->GetControlRotation();
	FVector spawnPosition = userActor->GetActorLocation() +
		(UKismetMathLibrary::GetForwardVector(spawnRotation) * 150.0f);
	const FVector spawnScale = FVector(1.0f, 1.0f, 1.0f);

	if (userActor->GetClass()->ImplementsInterface(UPlayerCharacter::StaticClass()))
	{		
		// Get Direction to Fire
		const FVector cameraPosition = IPlayerCharacter::Execute_GetCameraLocation(userActor);
		const FVector cameraForward = IPlayerCharacter::Execute_GetCameraForwardVector(userActor);

		// Line trace to get point in space the projectile will fire towards
		const FVector end = (cameraForward * 10000) + cameraPosition;
		FHitResult hitResult;
		TArray<AActor*> ignoreActors;
		ignoreActors.Add(userActor);

		spawnPosition = IPlayerCharacter::Execute_GetWeapon(userActor)->GetFirePoint()->GetComponentLocation();

		// Check for impact point to fire towards
		const bool traceHit = world->LineTraceSingleByChannel(hitResult, cameraPosition, end,
			ECC_Visibility);
		if (traceHit)
		{
			// If hit somewhere, get direction from the fire position to the impact point
			FVector fireDirection = hitResult.ImpactPoint - spawnPosition;
			fireDirection.Normalize();

			spawnRotation = UKismetMathLibrary::MakeRotFromX(fireDirection);
		} else
		{
			spawnRotation = UKismetMathLibrary::MakeRotFromX(cameraForward);
		}
	}

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

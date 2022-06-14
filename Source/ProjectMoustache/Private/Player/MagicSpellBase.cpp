// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MagicSpellBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/PlayerCharacter.h"
#include "Weapons/WeaponBase.h"

UMagicSpellBase::UMagicSpellBase()
{
}

bool UMagicSpellBase::Execute(APawn* userActor)
{
	if (userActor == nullptr)
	{
		return false;
	}
	
	const UWorld* world = GetWorld();
	if (world->GetTimeSeconds() < timeNextFire)
	{
		return false;
	}

	IPlayerCharacter* player = Cast<IPlayerCharacter>(userActor);
	if (player == nullptr)
	{
		CastSpell(userActor);
		return false;
	}

	if (!player->PlayAnim(castAnimation, animMontageSection))
	{
		CastSpell(userActor);
		return false;
	}

	return true;
}

void UMagicSpellBase::CastSpell(APawn* userActor)
{
	if (userActor == nullptr)
	{
		return;
	}

	if (spellProjectile == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
			TEXT("Error: Spell projectile not set!"));
		return;
	}
	
	UWorld* world = GetWorld();
	
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

		// Get position to cast to that is inline with the camera forward vector but inline with player
		const FVector dirFirePoint = spawnPosition - cameraPosition;
		const FVector castPoint = dirFirePoint.ProjectOnTo(cameraForward * 10000) + cameraPosition;

		// Line trace to get point in space the projectile will fire towards
		const FVector end = (castPoint * 10000) + castPoint;
		FHitResult hitResult;
		TArray<AActor*> ignoreActors;
		ignoreActors.Add(userActor);

		spawnPosition = IPlayerCharacter::Execute_GetWeapon(userActor)->GetFirePoint()->GetComponentLocation();

		// Check for impact point to fire towards
		const bool traceHit = world->LineTraceSingleByChannel(hitResult, castPoint, end,
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

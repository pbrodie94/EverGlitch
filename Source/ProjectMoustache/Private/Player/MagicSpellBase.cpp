// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MagicSpellBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
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

	// Get Direction to Fire
	const FVector cameraPosition = IPlayerCharacter::Execute_GetCameraLocation(userActor);
	const FVector cameraForward = IPlayerCharacter::Execute_GetCameraForwardVector(userActor);

	// Get position to cast to that is inline with the camera forward vector but inline with player
	const FVector firePoint = IPlayerCharacter::Execute_GetWeapon(userActor)->GetFirePoint()->GetComponentLocation();
	const FVector dirFirePoint = firePoint - cameraPosition;
	const FVector castPoint = dirFirePoint.ProjectOnTo(cameraForward * 10000) + cameraPosition;
	
	const FVector end = castPoint + (cameraForward * 10000);
	FHitResult hitResult;
	TArray<AActor*> ignore;
	const bool traceHit = UKismetSystemLibrary::LineTraceSingle(world, castPoint, end,
		UEngineTypes::ConvertToTraceType(ECC_Visibility), false, ignore,
		EDrawDebugTrace::None,hitResult, true);
	FVector fireDirection = cameraForward;
	if (traceHit)
	{
		fireDirection = hitResult.ImpactPoint - firePoint;
		fireDirection.Normalize();
	}

	// Spawn projectile
	const FRotator spawnRotation = UKismetMathLibrary::MakeRotFromX(fireDirection);

	FActorSpawnParameters spawnParams;
	spawnParams.Owner = userActor;
	spawnParams.Instigator = userActor;

	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = userActor;
	spawnParameters.Instigator = userActor;

	const FVector spawnScale = FVector(1.0f, 1.0f, 1.0f);

	world->SpawnActor<AActor>(spellProjectile, FTransform(spawnRotation, firePoint, spawnScale),
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

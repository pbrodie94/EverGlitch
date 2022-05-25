// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponBase.h"

#include "Damageable.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/PlayerCharacter.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	staffMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("StaffMesh"));
	staffMesh->SetupAttachment(RootComponent);

	firePoint = CreateDefaultSubobject<USceneComponent>(TEXT("FirePoint"));
	firePoint->SetupAttachment(staffMesh);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isFiring)
	{
		Fire();
	}
}

void AWeaponBase::Fire()
{
	if (projectile == nullptr || firePoint == nullptr)
	{
		return;
	}
	
	if (GetOwner() == nullptr || !GetOwner()->GetClass()->ImplementsInterface(UPlayerCharacter::StaticClass()))
	{
		return;
	}

	UWorld* world = GetWorld();
	if (world->GetTimeSeconds() < timeNextFire)
	{
		return;
	}
	
	// Get Direction to Fire
	const FVector cameraPosition = IPlayerCharacter::Execute_GetCameraLocation(GetOwner());
	const FVector cameraForward = IPlayerCharacter::Execute_GetCameraForwardVector(GetOwner());

	// Line trace to get point in space the projectile will fire towards
	const FVector end = (cameraForward * 10000) + cameraPosition;
	TArray<FHitResult> hitResults;
	TArray<AActor*> ignoreActors;
	ignoreActors.Add(GetOwner());

	FVector fireDirection = cameraForward;

	// Check for impact point to fire towards
	const bool traceHit = world->LineTraceMultiByChannel(hitResults, cameraPosition, end,
		ECC_Visibility);
	if (traceHit)
	{
		int index = 0;
		
		do
		{
			if (hitResults[index].GetActor() != nullptr &&
				hitResults[index].GetActor()->GetClass()->ImplementsInterface(UDamageable::StaticClass()))
			{
				if (IDamageable::Execute_GetIsDead(hitResults[index].GetActor()))
				{
					++index;
					continue;
				}
			}
			
			/*if (hitResult[index].GetActor() != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
					hitResult.GetActor()->GetName());
			}*/
		
			// If hit somewhere, get direction from the fire position to the impact point
			fireDirection = hitResults[index].ImpactPoint - firePoint->GetComponentLocation();
			fireDirection.Normalize();
			break;
			
		} while (index < hitResults.Num());
	}

	// Spawn projectile
	const FRotator spawnRotation = UKismetMathLibrary::MakeRotFromX(fireDirection);

	FActorSpawnParameters spawnParams;
	spawnParams.Owner = GetOwner();
	spawnParams.Instigator = GetInstigator();

	// Spawn projectile
	AProjectileBase* proj = world->SpawnActor<AProjectileBase>(projectile, FTransform(spawnRotation,
		firePoint->GetComponentLocation(), FVector(1.0f, 1.0f, 1.0f)), spawnParams);

	if (proj != nullptr)
	{
		proj->SetDamage(damage);
	}
	
	// Set the next time can fire
	timeNextFire = world->GetTimeSeconds() + fireRate;
}

bool AWeaponBase::OnFireDown_Implementation()
{
	/*const UWorld* world = GetWorld();
	if (world->GetTimeSeconds() < timeNextFire)
	{
		return false;
	}

	timeNextFire = world->GetTimeSeconds() + fireRate;*/
	isFiring = true;
	return true;
}

bool AWeaponBase::OnFireUp_Implementation()
{
	/*const UWorld* world = GetWorld();
	if (world->GetTimeSeconds() < timeNextFire)
	{
		return false;
	}

	timeNextFire = world->GetTimeSeconds() + fireRate;*/
	isFiring = false;
	return true;
}

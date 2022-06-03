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

	fireStartupDelay = 0.75f;
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
	
	const FVector start = UKismetMathLibrary::ProjectPointOnToPlane(cameraPosition,
		firePoint->GetComponentLocation(), cameraForward);
	const FVector end = start + (cameraForward * 10000);
	FHitResult hitResult;
	const bool traceHit = world->LineTraceSingleByChannel(hitResult, start, end, ECC_Visibility);
	FVector fireDirection = cameraForward;
	if (traceHit)
	{
		// Get angle
		FVector dirToImpact = hitResult.ImpactPoint - firePoint->GetComponentLocation();
		dirToImpact.Normalize();
		float angle = FVector::DotProduct(cameraForward, dirToImpact);
		angle = FMath::Acos(angle);
		angle = FMath::RadiansToDegrees(angle);

		if (angle < 10.0f)
		{
			fireDirection = hitResult.ImpactPoint - firePoint->GetComponentLocation();
			fireDirection.Normalize();
		}
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
	isFiring = true;
	
	IPlayerCharacter* player = Cast<IPlayerCharacter>(GetOwner());
	if (fireAnimation == nullptr || player == nullptr)
	{
		return true;
	}

	player->PlayAnim(fireAnimation, fireBegin);
	timeNextFire = GetWorld()->GetTimeSeconds() + fireStartupDelay;
	
	return true;
}

bool AWeaponBase::OnFireUp_Implementation()
{
	isFiring = false;
	
	IPlayerCharacter* player = Cast<IPlayerCharacter>(GetOwner());
	if (fireAnimation != nullptr && player != nullptr)
	{
		player->PlayAnim(fireAnimation, fireEnd);
	}
	
	
	return true;
}

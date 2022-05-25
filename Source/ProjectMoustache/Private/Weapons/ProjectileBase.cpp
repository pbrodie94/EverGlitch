// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileBase.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//Create sphere collider component and attach to root component
	sphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	sphereCollider->SetSphereRadius(25.0f);
	sphereCollider->SetCollisionResponseToChannels(ECR_Overlap);
	
	sphereCollider->SetupAttachment(RootComponent);

	//Create and initialize the projectile movement component
	projectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	projectileMovement->InitialSpeed = 5000;
	projectileMovement->MaxSpeed = 2500;
	projectileMovement->Velocity = FVector(5000.0f, 0, 0);
	projectileMovement->ProjectileGravityScale = 0;

	lifeSpan = 10;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (sphereCollider != nullptr)
	{
		sphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnComponentBeginOverlap);
	}
	
	if (lifeSpan > 0)
	{
		SetLifeSpan(lifeSpan);
	}
}

void AProjectileBase::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Projectile hit something"));
	
	if (OtherActor == GetOwner())
	{
		return;
	}

	if (OtherActor != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, OtherActor->GetName());
	}

	if (OtherActor != nullptr && OtherActor->ActorHasTag("Enemy"))
	{
		OtherActor->TakeDamage(damage, FDamageEvent(), GetOwner()->GetInstigatorController(),
			GetOwner());
	}

	this->Destroy();
}

/*
void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{
		return;
	}
	
	if (OtherActor != nullptr && OtherActor->ActorHasTag("Enemy"))
	{
		OtherActor->TakeDamage(damage, FDamageEvent(), GetOwner()->GetInstigatorController(),
			GetOwner());
	}

	this->Destroy();
}
*/


void AProjectileBase::SetProjectileSpeed(float speed)
{
	projectileMovement->MaxSpeed = speed;
}

void AProjectileBase::SetShouldBounce(bool shouldBounce)
{
	projectileMovement->bShouldBounce = shouldBounce;
}

void AProjectileBase::SetDamage(float damageAmount)
{
	if (damageAmount <= 0)
	{
		return;
	}
	
	damage = damageAmount;
}

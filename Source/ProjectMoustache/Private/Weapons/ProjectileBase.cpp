// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/ProjectileBase.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create sphere collider component and attach to root component
	sphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	sphereCollider->SetSphereRadius(25.0f);
	sphereCollider->SetupAttachment(RootComponent);
	sphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBase::OnBeginOverlapEvent);

	//Create the static mesh, and attach to the sphere collider
	projectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	projectileMesh->SetupAttachment(sphereCollider);

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

	if (lifeSpan > 0)
	{
		SetLifeSpan(lifeSpan);
	}
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileBase::OnBeginOverlapEvent
(UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp,
		int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	
}

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
	damage = damageAmount;
}


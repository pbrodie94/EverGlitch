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
	projectileMovement->Velocity = FVector(500.0f, 0, 0);
	projectileMovement->ProjectileGravityScale = 0;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileBase::Initialize(float projectileDamage, APawn* owner)
{
	this->damage = projectileDamage;
	this->myOwner = owner;
}

void AProjectileBase::OnBeginOverlapEvent
(UPrimitiveComponent* overlappedComp, class AActor* otherActor, class UPrimitiveComponent* otherComp,
		int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyBase.h"

#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AEnemyBase::AEnemyBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	maxHealth = 100;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (maxHealth <= 1)
	{
		maxHealth = 100;
	}

	currentHealth = startingHealth > 0 ? startingHealth : maxHealth;
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageAmount <= 0 || currentHealth <= 0)
	{
		return 0;
	}

	currentHealth -= DamageAmount;
	if (currentHealth <= 0)
	{
		currentHealth = 0;
		OnDied.Broadcast(this); 
		Die();
	}

	return DamageAmount;
}

void AEnemyBase::Die_Implementation()
{
	OnDied.Broadcast(this);
}

void AEnemyBase::SetMoveSpeed(bool running)
{
	GetCharacterMovement()->MaxWalkSpeed = running ? runSpeed : walkSpeed;
}

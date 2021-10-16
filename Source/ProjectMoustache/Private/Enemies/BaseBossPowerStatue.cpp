// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/BaseBossPowerStatue.h"

// Sets default values

ABaseBossPowerStatue::ABaseBossPowerStatue()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned

void ABaseBossPowerStatue::BeginPlay()
{
	Super::BeginPlay();

	if (defaultHealth <= 0)
	{
		health = 100;
	} else
	{
		health = defaultHealth;
	}
	
}

// Called every frame

void ABaseBossPowerStatue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ABaseBossPowerStatue::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	health -= DamageAmount;

	if (health <= 0)
	{
		//Destroyed
		//Notify call the delegate, then destroy
		OnStatueDestroyed.Execute(this);
		Destroy();
	}
	
	return DamageAmount;
}



// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactables/ItemBase.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	quantity = 1;
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (quantity <= 0 || !isStackable)
	{
		quantity = 1;
	}

	if (isStackable && maxStackSize <= 0)
	{
		isStackable = false;
	}
	
	if (isStackable && quantity > maxStackSize)
	{
		quantity = maxStackSize;
	}
}

bool AItemBase::Use_Implementation(AActor* userActor) { return true; }

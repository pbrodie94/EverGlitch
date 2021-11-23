// Fill out your copyright notice in the Description page of Project Settings.

#include "Interactables/ItemBase.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
}


void AItemBase::Tick(float DeltaTime)
{
	
}

bool AItemBase::Use_Implementation(AActor* userActor) { return true; }

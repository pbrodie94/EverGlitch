// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MagicSpellBase.h"

AMagicSpellBase::AMagicSpellBase()
{
}

void AMagicSpellBase::BeginPlay()
{
	Super::BeginPlay();
}


void AMagicSpellBase::Execute(APawn* userActor)
{
	UWorld* world = GetWorld();
	if (world->GetTimeSeconds() < timeNextFire)
	{
		return;
	}

	CastSpell(userActor);

	timeNextFire = world->GetTimeSeconds() + coolDownTime;
}


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

	timeCasted = world->GetTimeSeconds();
	timeNextFire = world->GetTimeSeconds() + coolDownTime;
}

float AMagicSpellBase::GetCoolDownRemaining()
{
	UWorld* world = GetWorld();
	if (world->GetTimeSeconds() > timeNextFire)
	{
		return 0;
	}

	return timeNextFire - world->GetTimeSeconds();
}


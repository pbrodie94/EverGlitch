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

	timeFired = world->GetTimeSeconds();
	timeNextFire = world->GetTimeSeconds() + coolDownTime;
}

float AMagicSpellBase::GetCooldownProgress()
{
	float time = (GetWorld()->GetTimeSeconds() - timeFired) / coolDownTime;
	return FMath::Clamp(time, 0.0f, 1.0f);
}

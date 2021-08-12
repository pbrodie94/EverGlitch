// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectMoustacheGameMode.h"
#include "ProjectMoustacheCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectMoustacheGameMode::AProjectMoustacheGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

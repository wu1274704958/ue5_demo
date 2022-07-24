// Copyright Epic Games, Inc. All Rights Reserved.

#include "ue5_demoGameMode.h"
#include "ue5_demoCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include <format>
#include <Logging/LogVerbosity.h>

Aue5_demoGameMode::Aue5_demoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

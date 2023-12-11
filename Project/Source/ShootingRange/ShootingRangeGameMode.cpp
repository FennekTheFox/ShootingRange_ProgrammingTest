// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootingRangeGameMode.h"
#include "ShootingRangeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AShootingRangeGameMode::AShootingRangeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

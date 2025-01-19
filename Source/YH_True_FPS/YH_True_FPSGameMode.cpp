// Copyright Epic Games, Inc. All Rights Reserved.

#include "YH_True_FPSGameMode.h"
#include "YH_True_FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AYH_True_FPSGameMode::AYH_True_FPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

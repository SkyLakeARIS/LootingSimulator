// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LootingSimulatorGameMode.generated.h"

/**
 *  Simple GameMode for a first person game
 */
UCLASS(abstract)
class ALootingSimulatorGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALootingSimulatorGameMode();
};




// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ue5_demoGameMode.generated.h"

UCLASS(minimalapi)
class Aue5_demoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	Aue5_demoGameMode();
	virtual void StartPlay() override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
};




// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "UObject/Object.h"
#include "AMySpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class UE5_DEMO_API AMySpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()
public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;
};

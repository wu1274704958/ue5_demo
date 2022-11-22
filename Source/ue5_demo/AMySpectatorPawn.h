// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpectatorPawn.h"
#include "GameFramework/SpringArmComponent.h"
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
	AMySpectatorPawn();
	virtual void PossessedBy(AController* NewController) override;
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
protected:
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere,Category=Input)
	float TurnRateGamepad;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;
};

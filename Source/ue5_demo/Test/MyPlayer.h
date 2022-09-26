// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MyPlayer.generated.h"

/**
 * 
 */
UCLASS()
class UE5_DEMO_API UMyPlayer : public ULocalPlayer
{
	GENERATED_BODY()
public:
	UMyPlayer();
	virtual void SwitchController(APlayerController* PC) override;
protected:
	virtual FSceneView* CalcSceneView(FSceneViewFamily* ViewFamily, FVector& OutViewLocation, FRotator& OutViewRotation, FViewport* Viewport, FViewElementDrawer* ViewDrawer, int32 StereoViewIndex) override;
};

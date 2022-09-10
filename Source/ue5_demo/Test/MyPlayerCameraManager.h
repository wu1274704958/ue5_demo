// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyPlayerCameraManager.generated.h"

UCLASS()
class UE5_DEMO_API AMyPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyPlayerCameraManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams) override;
protected:
	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;
};

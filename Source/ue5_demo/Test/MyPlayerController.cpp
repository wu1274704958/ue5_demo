// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"

#include "MyPlayerCameraManager.h"


// Sets default values
AMyPlayerController::AMyPlayerController()
{
	PlayerCameraManagerClass = AMyPlayerCameraManager::StaticClass();
}

// Called when the game starts or when spawned
void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


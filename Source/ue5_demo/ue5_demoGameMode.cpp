// Copyright Epic Games, Inc. All Rights Reserved.

#include "ue5_demoGameMode.h"
#include "ue5_demoCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include <format>
#include <Logging/LogVerbosity.h>
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "TestCpp2a.h"
#include "Test/MyPlayerController.h"
#include "Camera/CameraActor.h"
#include "Test/MyPlayer.h"

DEFINE_LOG_CATEGORY_STATIC(LogFindCamera, Warning, All)

Aue5_demoGameMode::Aue5_demoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	static ConstructorHelpers::FClassFinder<APawn> SpectatorPawnClass(TEXT("/Game/Test/BP_SpectatorPawn"));
	
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = SpectatorPawnClass.Class;
		PlayerControllerClass = AMyPlayerController::StaticClass();
	}
}

void Aue5_demoGameMode::StartPlay()
{
	Super::StartPlay();
	bool noNone = true;
	for(TActorIterator<AActor> it(GetWorld());it;++it)
	{
		auto cam = dynamic_cast<ACameraActor*>(*it);
		if(cam != nullptr)
		{
			noNone = false;
			auto clsName = it->GetClass()->GetFullName();
			UE_LOG(LogFindCamera,Warning,TEXT("Find Camera %s in %s"),*cam->GetName(),*clsName);
			it->Destroy();
		}
	}
	if(noNone)
		UE_LOG(LogFindCamera,Warning,TEXT("Not Find Camera"));
}

void Aue5_demoGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	UWorld* World = GetWorld();
	World->GetGameInstance()->GetEngine()->LocalPlayerClass = UMyPlayer::StaticClass();
	Super::InitGame(MapName, Options, ErrorMessage);
}

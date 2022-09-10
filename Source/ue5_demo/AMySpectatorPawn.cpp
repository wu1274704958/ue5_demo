// Fill out your copyright notice in the Description page of Project Settings.



#include "AMySpectatorPawn.h"
#include "Camera/CameraComponent.h"

DEFINE_LOG_CATEGORY_STATIC(MySpectatorPawn, Warning, All)

void AMySpectatorPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	const auto cls_name = NewController->GetClass()->GetFullName();
	UE_LOG(MySpectatorPawn,Warning,TEXT("PossessedBy %s"),*cls_name);
}

void AMySpectatorPawn::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	if (bFindCameraComponentWhenViewTarget)
	{
		// Look for the first active camera component and use that for the view
		TInlineComponentArray<UCameraComponent*> Cameras;
		GetComponents<UCameraComponent>(/*out*/ Cameras);

		for (UCameraComponent* CameraComponent : Cameras)
		{
			if (CameraComponent->IsActive())
			{
				CameraComponent->GetCameraView(DeltaTime, OutResult);
				return;
			}
		}
	}

	GetActorEyesViewPoint(OutResult.Location, OutResult.Rotation);
}

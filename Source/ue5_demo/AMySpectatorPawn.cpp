// Fill out your copyright notice in the Description page of Project Settings.



#include "AMySpectatorPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

DEFINE_LOG_CATEGORY_STATIC(MySpectatorPawn, Warning, All)

AMySpectatorPawn::AMySpectatorPawn()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 10.0f;
	CameraBoom->bUsePawnControlRotation = true;
	
	TurnRateGamepad = 50.f;
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

}

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

void AMySpectatorPawn::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	InInputComponent->BindAxis("Turn Right / Left Mouse",this,&AMySpectatorPawn::TurnAtRate);
	InInputComponent->BindAxis("Look Up / Down Mouse",this,&AMySpectatorPawn::LookUpAtRate);
	InInputComponent->BindAxis("Move Forward / Backward", this, &AMySpectatorPawn::MoveForward);
	InInputComponent->BindAxis("Move Right / Left", this, &AMySpectatorPawn::MoveRight);
}

void AMySpectatorPawn::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMySpectatorPawn::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AMySpectatorPawn::MoveForward(float Val)
{
	auto rot = GetControlRotation();
	AddMovementInput(FRotationMatrix(rot).GetScaledAxis(EAxis::X),Val);
}

void AMySpectatorPawn::MoveRight(float Val)
{
	auto rot = GetControlRotation();
	AddMovementInput(FRotationMatrix(rot).GetScaledAxis(EAxis::Y),Val);
}

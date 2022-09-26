// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerCameraManager.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"

DEFINE_LOG_CATEGORY_STATIC(MyPlayerCameraManager, Warning, All)
// Sets default values
AMyPlayerCameraManager::AMyPlayerCameraManager()
{
	
}

// Called when the game starts or when spawned
void AMyPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(MyPlayerCameraManager,Warning,TEXT("AMyPlayerCameraManager()"));
}

// Called every frame
void AMyPlayerCameraManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	if ((PendingViewTarget.Target != NULL) && BlendParams.bLockOutgoing && OutVT.Equal(ViewTarget))
	{
		return;
	}

	// store previous POV, in case we need it later
	FMinimalViewInfo OrigPOV = OutVT.POV;

	//@TODO: CAMERA: Should probably reset the view target POV fully here
	OutVT.POV.FOV = DefaultFOV;
	OutVT.POV.OrthoWidth = DefaultOrthoWidth;
	OutVT.POV.AspectRatio = DefaultAspectRatio;
	OutVT.POV.bConstrainAspectRatio = bDefaultConstrainAspectRatio;
	OutVT.POV.bUseFieldOfViewForLOD = true;
	OutVT.POV.ProjectionMode = bIsOrthographic ? ECameraProjectionMode::Orthographic : ECameraProjectionMode::Perspective;
	OutVT.POV.PostProcessSettings.SetBaseValues();
	OutVT.POV.PostProcessBlendWeight = 1.0f;


	bool bDoNotApplyModifiers = false;

	if (ACameraActor* CamActor = Cast<ACameraActor>(OutVT.Target))
	{
		// Viewing through a camera actor.
		CamActor->GetCameraComponent()->GetCameraView(DeltaTime, OutVT.POV);
	}
	else
	{

		static const FName NAME_Fixed = FName(TEXT("Fixed"));
		static const FName NAME_ThirdPerson = FName(TEXT("ThirdPerson"));
		static const FName NAME_FreeCam = FName(TEXT("FreeCam"));
		static const FName NAME_FreeCam_Default = FName(TEXT("FreeCam_Default"));
		static const FName NAME_FirstPerson = FName(TEXT("FirstPerson"));

		if (CameraStyle == NAME_Fixed)
		{
			// do not update, keep previous camera position by restoring
			// saved POV, in case CalcCamera changes it but still returns false
			OutVT.POV = OrigPOV;

			// don't apply modifiers when using this debug camera mode
			bDoNotApplyModifiers = true;
		}
		else if (CameraStyle == NAME_ThirdPerson || CameraStyle == NAME_FreeCam || CameraStyle == NAME_FreeCam_Default)
		{
			// Simple third person view implementation
			FVector Loc = OutVT.Target->GetActorLocation();
			FRotator Rotator = OutVT.Target->GetActorRotation();

			if (OutVT.Target == PCOwner)
			{
				Loc = PCOwner->GetFocalLocation();
			}

			// Take into account Mesh Translation so it takes into account the PostProcessing we do there.
			// @fixme, can crash in certain BP cases where default mesh is null
//			APawn* TPawn = Cast<APawn>(OutVT.Target);
// 			if ((TPawn != NULL) && (TPawn->Mesh != NULL))
// 			{
// 				Loc += FQuatRotationMatrix(OutVT.Target->GetActorQuat()).TransformVector(TPawn->Mesh->RelativeLocation - GetDefault<APawn>(TPawn->GetClass())->Mesh->RelativeLocation);
// 			}

			//OutVT.Target.GetActorEyesViewPoint(Loc, Rot);
			if( CameraStyle == NAME_FreeCam || CameraStyle == NAME_FreeCam_Default )
			{
				Rotator = PCOwner->GetControlRotation();
			}

			FVector Pos = Loc + ViewTargetOffset + FRotationMatrix(Rotator).TransformVector(FreeCamOffset) - Rotator.Vector() * FreeCamDistance;
			FCollisionQueryParams BoxParams(SCENE_QUERY_STAT(FreeCam), false, this);
			BoxParams.AddIgnoredActor(OutVT.Target);
			FHitResult Result;

			GetWorld()->SweepSingleByChannel(Result, Loc, Pos, FQuat::Identity, ECC_Camera, FCollisionShape::MakeBox(FVector(12.f)), BoxParams);
			OutVT.POV.Location = !Result.bBlockingHit ? Pos : Result.Location;
			OutVT.POV.Rotation = Rotator;

			// don't apply modifiers when using this debug camera mode
			bDoNotApplyModifiers = true;
		}
		else if (CameraStyle == NAME_FirstPerson)
		{
			// Simple first person, view through viewtarget's 'eyes'
			OutVT.Target->GetActorEyesViewPoint(OutVT.POV.Location, OutVT.POV.Rotation);
	
			// don't apply modifiers when using this debug camera mode
			bDoNotApplyModifiers = true;
		}
		else
		{
			UpdateViewTargetInternal(OutVT, DeltaTime);
		}
	}

	if (!bDoNotApplyModifiers || bAlwaysApplyModifiers)
	{
		// Apply camera modifiers at the end (view shakes for example)
		ApplyCameraModifiers(DeltaTime, OutVT.POV);
	}

	// Synchronize the actor with the view target results
	SetActorLocationAndRotation(OutVT.POV.Location, OutVT.POV.Rotation, false);

	UpdateCameraLensEffects(OutVT);
}

void AMyPlayerCameraManager::SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	Super::SetViewTarget(NewViewTarget, TransitionParams);
	UE_LOG(MyPlayerCameraManager,Warning,TEXT("SetViewTarget(%s)"),*NewViewTarget->GetFullName());
}

void AMyPlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	if (OutVT.Target)
	{
		FVector OutLocation;
		FRotator OutRotation;
		float OutFOV;

		if (BlueprintUpdateCamera(OutVT.Target, OutLocation, OutRotation, OutFOV))
		{
			OutVT.POV.Location = OutLocation;
			OutVT.POV.Rotation = OutRotation;
			OutVT.POV.FOV = OutFOV;
		}
		else
		{
			OutVT.Target->CalcCamera(DeltaTime, OutVT.POV);
		}
	}
}

void AMyPlayerCameraManager::DoUpdateCamera(float DeltaTime)
{
	FMinimalViewInfo NewPOV = ViewTarget.POV;

	// update color scale interpolation
	if (bEnableColorScaleInterp)
	{
		float BlendPct = FMath::Clamp((GetWorld()->TimeSeconds - ColorScaleInterpStartTime) / ColorScaleInterpDuration, 0.f, 1.0f);
		ColorScale = FMath::Lerp(OriginalColorScale, DesiredColorScale, BlendPct);
		// if we've maxed
		if (BlendPct == 1.0f)
		{
			// disable further interpolation
			bEnableColorScaleInterp = false;
		}
	}

	// Don't update outgoing viewtarget during an interpolation when bLockOutgoing is set.
	if ((PendingViewTarget.Target == NULL) || !BlendParams.bLockOutgoing)
	{
		// Update current view target
		ViewTarget.CheckViewTarget(PCOwner);
		UpdateViewTarget(ViewTarget, DeltaTime);
	}

	// our camera is now viewing there
	NewPOV = ViewTarget.POV;

	// if we have a pending view target, perform transition from one to another.
	if (PendingViewTarget.Target != NULL)
	{
		BlendTimeToGo -= DeltaTime;

		// Update pending view target
		PendingViewTarget.CheckViewTarget(PCOwner);
		UpdateViewTarget(PendingViewTarget, DeltaTime);

		// blend....
		if (BlendTimeToGo > 0)
		{
			float DurationPct = (BlendParams.BlendTime - BlendTimeToGo) / BlendParams.BlendTime;

			float BlendPct = 0.f;
			switch (BlendParams.BlendFunction)
			{
			case VTBlend_Linear:
				BlendPct = FMath::Lerp(0.f, 1.f, DurationPct);
				break;
			case VTBlend_Cubic:
				BlendPct = FMath::CubicInterp(0.f, 0.f, 1.f, 0.f, DurationPct);
				break;
			case VTBlend_EaseIn:
				BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, BlendParams.BlendExp));
				break;
			case VTBlend_EaseOut:
				BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, 1.f / BlendParams.BlendExp));
				break;
			case VTBlend_EaseInOut:
				BlendPct = FMath::InterpEaseInOut(0.f, 1.f, DurationPct, BlendParams.BlendExp);
				break;
			case VTBlend_PreBlended:
				BlendPct = 1.0f;
				break;
			default:
				break;
			}

			// Update pending view target blend
			NewPOV = ViewTarget.POV;
			NewPOV.BlendViewInfo(PendingViewTarget.POV, BlendPct);//@TODO: CAMERA: Make sure the sense is correct!  BlendViewTargets(ViewTarget, PendingViewTarget, BlendPct);
		}
		else
		{
			// we're done blending, set new view target
			ViewTarget = PendingViewTarget;

			// clear pending view target
			PendingViewTarget.Target = NULL;

			BlendTimeToGo = 0;

			// our camera is now viewing there
			NewPOV = PendingViewTarget.POV;

			OnBlendComplete().Broadcast();
		}
	}

	if (bEnableFading)
	{
		if (bAutoAnimateFade)
		{
			FadeTimeRemaining = FMath::Max(FadeTimeRemaining - DeltaTime, 0.0f);
			if (FadeTime > 0.0f)
			{
				FadeAmount = FadeAlpha.X + ((1.f - FadeTimeRemaining / FadeTime) * (FadeAlpha.Y - FadeAlpha.X));
			}

			if ((bHoldFadeWhenFinished == false) && (FadeTimeRemaining <= 0.f))
			{
				// done
				StopCameraFade();
			}
		}

		if (bFadeAudio)
		{
			ApplyAudioFade();
		}
	}
	
	if (AllowPhotographyMode())
	{
		const bool bPhotographyCausedCameraCut = UpdatePhotographyCamera(NewPOV);
		bGameCameraCutThisFrame = bGameCameraCutThisFrame || bPhotographyCausedCameraCut;
	}
	//在这里换掉位置数据是可行的
	//NewPOV.Location = FVector::One(); 
	// Cache results 应该是这里处理了Camera的位置和旋转
	FillCameraCache(NewPOV);
	
}


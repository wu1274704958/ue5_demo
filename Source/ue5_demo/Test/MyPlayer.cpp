// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayer.h"

#include "SceneViewExtension.h"
#include "Engine/DebugCameraController.h"
DEFINE_LOG_CATEGORY_STATIC(MyPlayer, Warning, All)

UMyPlayer::UMyPlayer()
{
	UE_LOG(MyPlayer,Warning,TEXT("MyPlayer()"));
}

void UMyPlayer::SwitchController(APlayerController* PC)
{
	auto name = PC->GetFullName();
	UE_LOG(MyPlayer,Warning,TEXT("SwitchController %s"),*name);
	Super::SwitchController(PC);
}

FSceneView* UMyPlayer::CalcSceneView(FSceneViewFamily* ViewFamily, FVector& OutViewLocation, FRotator& OutViewRotation,
	FViewport* Viewport, FViewElementDrawer* ViewDrawer, int32 StereoViewIndex)
{
	FSceneViewInitOptions ViewInitOptions;

	if (!CalcSceneViewInitOptions(ViewInitOptions, Viewport, ViewDrawer, StereoViewIndex))
	{
		return nullptr;
	}

	// Get the viewpoint...technically doing this twice
	// but it makes GetProjectionData better
	FMinimalViewInfo ViewInfo;
	GetViewPoint(ViewInfo);
	ViewInitOptions.ViewLocation = ViewInfo.Location;
	ViewInitOptions.ViewRotation = ViewInfo.Rotation;
	ViewInitOptions.bUseFieldOfViewForLOD = ViewInfo.bUseFieldOfViewForLOD;
	ViewInitOptions.FOV = ViewInfo.FOV;
	ViewInitOptions.DesiredFOV = ViewInfo.DesiredFOV;

	// Fill out the rest of the view init options
	ViewInitOptions.ViewFamily = ViewFamily;

	if (!PlayerController->bRenderPrimitiveComponents)
	{
		// Emplaces an empty show only primitive list.
		ViewInitOptions.ShowOnlyPrimitives.Emplace();
	}
	else
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_BuildHiddenComponentList);
		PlayerController->BuildHiddenComponentList(ViewInfo.Location, /*out*/ ViewInitOptions.HiddenPrimitives);
	}

	//@TODO: SPLITSCREEN: This call will have an issue with splitscreen, as the show flags are shared across the view family
	EngineShowFlagOrthographicOverride( ViewInitOptions.IsPerspectiveProjection(), ViewFamily->EngineShowFlags );

	FSceneView* const View = new FSceneView(ViewInitOptions);

	OutViewLocation = View->ViewLocation;
	OutViewRotation = View->ViewRotation;
	// Pass on the previous view transform from the view info (probably provided by the camera if set)
	View->PreviousViewTransform = ViewInfo.PreviousViewTransform;

	ViewFamily->Views.Add(View);

	{
		View->StartFinalPostprocessSettings(ViewInfo.Location);

		// CameraAnim override
		if (PlayerController->PlayerCameraManager)
		{
			TArray<FPostProcessSettings> const* CameraAnimPPSettings;
			TArray<float> const* CameraAnimPPBlendWeights;
			PlayerController->PlayerCameraManager->GetCachedPostProcessBlends(CameraAnimPPSettings, CameraAnimPPBlendWeights);

			for (int32 PPIdx = 0; PPIdx < CameraAnimPPBlendWeights->Num(); ++PPIdx)
			{
				View->OverridePostProcessSettings( (*CameraAnimPPSettings)[PPIdx], (*CameraAnimPPBlendWeights)[PPIdx]);
			}
		}

		//	CAMERA OVERRIDE
		View->OverridePostProcessSettings(ViewInfo.PostProcessSettings, ViewInfo.PostProcessBlendWeight);

		if (PlayerController->PlayerCameraManager)
		{
			PlayerController->PlayerCameraManager->UpdatePhotographyPostProcessing(View->FinalPostProcessSettings);
		}

		if (GEngine->StereoRenderingDevice.IsValid())
		{
			FPostProcessSettings StereoDeviceOverridePostProcessinSettings;
			float BlendWeight = 1.0f;
			bool StereoSettingsAvailable = GEngine->StereoRenderingDevice->OverrideFinalPostprocessSettings(&StereoDeviceOverridePostProcessinSettings, View->StereoPass, View->StereoViewIndex, BlendWeight);
			if (StereoSettingsAvailable)
			{
				View->OverridePostProcessSettings(StereoDeviceOverridePostProcessinSettings, BlendWeight);
			}
		}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
		ADebugCameraController* DebugCameraController = Cast<ADebugCameraController>(PlayerController);
		if (DebugCameraController != nullptr)
		{
			DebugCameraController->UpdateVisualizeBufferPostProcessing(View->FinalPostProcessSettings);
		}
#endif

		View->EndFinalPostprocessSettings(ViewInitOptions);
	}

	for (int ViewExt = 0; ViewExt < ViewFamily->ViewExtensions.Num(); ViewExt++)
	{
		ViewFamily->ViewExtensions[ViewExt]->SetupView(*ViewFamily, *View);
	}

	return View;
}


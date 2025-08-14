//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

#include "PXR_DPHMD.h"
#include "CoreMinimal.h"
#include "ClearQuad.h"
#include "DefaultSpectatorScreenController.h"
#include "ScreenRendering.h"
#include "PXR_Log.h"


#if PLATFORM_WINDOWS
#include "DynamicRHI.h"
#include "D3D11RHI.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <d3d11.h>
#endif

#include "Slate/SceneViewport.h"
#include "Engine/GameEngine.h"
#include "GameFramework/PlayerController.h"
#include "IHeadMountedDisplayVulkanExtensions.h"
#include "HeadMountedDisplayFunctionLibrary.h"

#include "CanvasItem.h"
#include "CommonRenderResources.h"
#include "ISettingsModule.h"
#include "PXR_DPSettings.h"
#include "GameFramework/GameUserSettings.h"

#if WITH_EDITOR
#include "Editor/UnrealEd/Classes/Editor/EditorEngine.h"
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE( a ) ( sizeof( ( a ) ) / sizeof( ( a )[ 0 ] ) )
#endif

static constexpr float PreviewFov = 101.f;

DEFINE_LOG_CATEGORY(LogPICODP);
/** Helper function for acquiring the appropriate FSceneViewport */
FSceneViewport* FindSceneViewport()
{
	if (!GIsEditor)
	{
		UGameEngine* GameEngine = Cast<UGameEngine>(GEngine);
		return GameEngine->SceneViewport.Get();
	}
#if WITH_EDITOR
	else
	{
		UEditorEngine* EditorEngine = CastChecked<UEditorEngine>(GEngine);
		FSceneViewport* PIEViewport = (FSceneViewport*)EditorEngine->GetPIEViewport();
		if (PIEViewport != nullptr && PIEViewport->IsStereoRenderingAllowed())
		{
			// PIE is setup for stereo rendering
			return PIEViewport;
		}
		else
		{
			// Check to see if the active editor viewport is drawing in stereo mode
			// @todo vreditor: Should work with even non-active viewport!
			FSceneViewport* EditorViewport = (FSceneViewport*)EditorEngine->GetActiveViewport();
			if (EditorViewport != nullptr && EditorViewport->IsStereoRenderingAllowed())
			{
				return EditorViewport;
			}
		}
	}
#endif
	return nullptr;
}

//---------------------------------------------------
// PICODP Plugin Implementation
//---------------------------------------------------

class FPICODPPlugin : public IPICOXRDPModule
{
	/** IHeadMountedDisplayModule implementation */
	virtual TSharedPtr<class IXRTrackingSystem, ESPMode::ThreadSafe> CreateTrackingSystem() override;

	virtual FString GetModuleKeyName() const override
	{
		float ModulePriority;
		if (!GConfig->GetFloat(TEXT("HMDPluginPriority"), *FString(TEXT("PICOXRPreview")), ModulePriority, GEngineIni))
		{
			ModulePriority = 50.0f;
			GConfig->SetFloat(TEXT("HMDPluginPriority"), *FString(TEXT("PICOXRPreview")), ModulePriority, GEngineIni);
		}
		
		return FString(TEXT("PICOXRPreview"));
	}
	
	
	void* LibusbDllHandle;
	void* ps_baseDllHandle;
	void* ps_commonDllHandle;
	void* ps_driver_runtime;

	bool LoadPICODPModule();
	void UnloadPICODPModule();

public:
	FPICODPPlugin()
	{
	}

	virtual void StartupModule() override
	{
		IHeadMountedDisplayModule::StartupModule();
		LoadPICODPModule();
	}

	virtual void ShutdownModule() override
	{
		IHeadMountedDisplayModule::ShutdownModule();
		UnloadPICODPModule();
	}

	bool Initialize()
	{
		PXR_LOGD(PxrUnreal, "PXR_LivePreview Initialize!");
		return true;
	}

	virtual bool PreInit() override
	{
		return true;
	}

	virtual bool IsHMDConnected() override
	{
		return true;
	}

#if PLATFORM_WINDOWS
	enum class D3DApiLevel
	{
		Undefined,
		Direct3D11,
		Direct3D12
	};

	static inline D3DApiLevel GetD3DApiLevel()
	{
		FString RHIString;
		{
			FString HardwareDetails = FHardwareInfo::GetHardwareDetailsString();
			FString RHILookup = NAME_RHI.ToString() + TEXT("=");

			PXR_LOGD(PxrUnreal, "PXR_LivePreview RHIString:%s", *RHIString);
			if (!FParse::Value(*HardwareDetails, *RHILookup, RHIString))
			{
				// RHI might not be up yet. Let's check the command-line and see if DX12 was specified.
				// This will get hit on startup since we don't have RHI details during stereo device bringup. 
				// This is not a final fix; we should probably move the stereo device init to later on in startup.
				bool bForceD3D12 = FParse::Param(FCommandLine::Get(), TEXT("d3d12")) || FParse::Param(FCommandLine::Get(), TEXT("dx12"));
				return bForceD3D12 ? D3DApiLevel::Direct3D12 : D3DApiLevel::Direct3D11;
			}
		}

		if (RHIString == TEXT("D3D11"))
		{
			return D3DApiLevel::Direct3D11;
		}
		if (RHIString == TEXT("D3D12"))
		{
			return D3DApiLevel::Direct3D12;
		}

		return D3DApiLevel::Undefined;
	}

#endif

private:
	TSharedPtr<IHeadMountedDisplayVulkanExtensions, ESPMode::ThreadSafe> VulkanExtensions;
};

void FPICODPPlugin::UnloadPICODPModule()
{
	if (LibusbDllHandle)
	{
		UE_LOG(LogHMD, Log, TEXT("Freeing LibusbDllHandle."));
		FPlatformProcess::FreeDllHandle(LibusbDllHandle);
		LibusbDllHandle = nullptr;
	}
	if (ps_baseDllHandle)
	{
		UE_LOG(LogHMD, Log, TEXT("Freeing ps_baseDllHandle."));
		FPlatformProcess::FreeDllHandle(ps_baseDllHandle);
		ps_baseDllHandle = nullptr;
	}
	if (ps_commonDllHandle)
	{
		UE_LOG(LogHMD, Log, TEXT("Freeing ps_commonDllHandle."));
		FPlatformProcess::FreeDllHandle(ps_commonDllHandle);
		ps_commonDllHandle = nullptr;
	}
	if (ps_driver_runtime)
	{
		UE_LOG(LogHMD, Log, TEXT("Freeing ps_driver_runtime."));
		FPlatformProcess::FreeDllHandle(ps_driver_runtime);
		ps_driver_runtime = nullptr;
	}
}

bool FPICODPPlugin::LoadPICODPModule()
{
#if PLATFORM_WINDOWS
#if PLATFORM_64BITS
	FString StreamerDLLDir = FPaths::ProjectPluginsDir() / FString::Printf(TEXT("PICOLivePreview/Source/ThirdParty/bin/"));
	FPlatformProcess::PushDllDirectory(*StreamerDLLDir);
	LibusbDllHandle = FPlatformProcess::GetDllHandle(*(StreamerDLLDir + "libusb-1.0.dll"));
	ps_baseDllHandle = FPlatformProcess::GetDllHandle(*(StreamerDLLDir + "ps_base.dll"));
	ps_commonDllHandle = FPlatformProcess::GetDllHandle(*(StreamerDLLDir + "ps_common.dll"));
	ps_driver_runtime = FPlatformProcess::GetDllHandle(*(StreamerDLLDir + "ps_driver_runtime.dll"));
	FPlatformProcess::PopDllDirectory(*StreamerDLLDir);

	if (!LibusbDllHandle
		|| !ps_baseDllHandle
		|| !ps_commonDllHandle
		|| !ps_driver_runtime)
	{
		UE_LOG(LogHMD, Log, TEXT("Failed to load PICODP library."));
		return false;
	}

#endif
#endif
	return true;
}

IMPLEMENT_MODULE(FPICODPPlugin, PICOXRDPHMD)

TSharedPtr<class IXRTrackingSystem, ESPMode::ThreadSafe> FPICODPPlugin::CreateTrackingSystem()
{
	PXR_LOGD(PxrUnreal, "PXR_LivePreview Start CreateTrackingSystem!");

	auto level = FPICODPPlugin::GetD3DApiLevel();

	if (level == FPICODPPlugin::D3DApiLevel::Direct3D11)
	{
		TSharedPtr<FPICOXRHMDDP, ESPMode::ThreadSafe> PICODPHMD = FSceneViewExtensions::NewExtension<FPICOXRHMDDP>(this);
		if (PICODPHMD)
		{
			return PICODPHMD;
		}
	}
	return nullptr;
}


//---------------------------------------------------
// PICODP IHeadMountedDisplay Implementation
//---------------------------------------------------

#if STEAMVR_SUPPORTED_PLATFORMS


bool FPICOXRHMDDP::IsHMDConnected()
{
	return true;
}

bool FPICOXRHMDDP::IsHMDEnabled() const
{
	return bHmdEnabled;
}

EHMDWornState::Type FPICOXRHMDDP::GetHMDWornState()
{
	return HmdWornState;
}

void FPICOXRHMDDP::EnableHMD(bool enable)
{
	bHmdEnabled = enable;

	if (!bHmdEnabled)
	{
		EnableStereo(false);
	}
}

bool FPICOXRHMDDP::GetHMDMonitorInfo(MonitorInfo& MonitorDesc)
{
	MonitorDesc.MonitorName = "";
	MonitorDesc.MonitorId = 0;
	MonitorDesc.DesktopX = MonitorDesc.DesktopY = MonitorDesc.ResolutionX = MonitorDesc.ResolutionY = 0;
	return false;
}

void FPICOXRHMDDP::GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const
{
	OutHFOVInDegrees = 0.0f;
	OutVFOVInDegrees = 0.0f;
}

bool FPICOXRHMDDP::DoesSupportPositionalTracking() const
{
	return true;
}

bool FPICOXRHMDDP::HasValidTrackingPosition()
{
	return true;
}

bool FPICOXRHMDDP::GetTrackingSensorProperties(int32 SensorId, FQuat& OutOrientation, FVector& OutOrigin, FXRSensorProperties& OutSensorProperties)
{
	OutOrigin = FVector::ZeroVector;
	OutOrientation = FQuat::Identity;
	OutSensorProperties = FXRSensorProperties();
	return true;
}

FString FPICOXRHMDDP::GetTrackedDevicePropertySerialNumber(int32 DeviceId)
{
	return FString();
}

void FPICOXRHMDDP::SetInterpupillaryDistance(float NewInterpupillaryDistance)
{
}

float FPICOXRHMDDP::GetInterpupillaryDistance() const
{
	return 0.064f;
}

bool FPICOXRHMDDP::GetCurrentPose(int32 DeviceId, FQuat& CurrentOrientation, FVector& CurrentPosition)
{
	FQuat tempCurrentOrientation = FQuat::Identity;

	if (FPICOXRDPManager::IsStreaming())
	{
		FPICOXRDPManager::GetHMDPositionAndRotation(CurrentPosition, tempCurrentOrientation);
		CurrentPosition = FVector(-CurrentPosition.Z, CurrentPosition.X, CurrentPosition.Y) * 100;
		CurrentOrientation.X = tempCurrentOrientation.Z;
		CurrentOrientation.Y = -tempCurrentOrientation.X;
		CurrentOrientation.Z = -tempCurrentOrientation.Y;
		CurrentOrientation.W = tempCurrentOrientation.W;
		//Todo:Prevent crashes caused by invalid Unnormalized data
		CurrentOrientation.Normalize();

		return true;
	}
	return false;
}

void FPICOXRHMDDP::SetTrackingOrigin(EHMDTrackingOrigin::Type NewOrigin)
{
}

EHMDTrackingOrigin::Type FPICOXRHMDDP::GetTrackingOrigin() const
{
	return EHMDTrackingOrigin::LocalFloor;
}

bool FPICOXRHMDDP::GetFloorToEyeTrackingTransform(FTransform& OutStandingToSeatedTransform) const
{
	bool bSuccess = false;
	return bSuccess;
}

FVector2D FPICOXRHMDDP::GetPlayAreaBounds(EHMDTrackingOrigin::Type Origin) const
{
	FVector2D Bounds=FVector2D();
	if (Origin == EHMDTrackingOrigin::Stage)
	{
		return Bounds;
	}

	return FVector2D::ZeroVector;
}

void FPICOXRHMDDP::RecordAnalytics()
{
}

float FPICOXRHMDDP::GetWorldToMetersScale() const
{
	return 100.0f;
}


bool FPICOXRHMDDP::EnumerateTrackedDevices(TArray<int32>& TrackedIds, EXRTrackedDeviceType DeviceType)
{
	TrackedIds.Empty();
	if (DeviceType == EXRTrackedDeviceType::Any || DeviceType == EXRTrackedDeviceType::HeadMountedDisplay)
	{
		TrackedIds.Add(IXRTrackingSystem::HMDDeviceId);
		return true;
	}
	return false;
}


bool FPICOXRHMDDP::IsTracking(int32 DeviceId)
{
	return true;
}

bool FPICOXRHMDDP::IsChromaAbCorrectionEnabled() const
{
	return false;
}

void FPICOXRHMDDP::OnBeginPlay(FWorldContext& InWorldContext)
{
#if WITH_EDITOR
	if (!InitializedSucceeded)
	{
		return;
	}

	if (GIsEditor)
	{
		UEditorEngine* EdEngine = Cast<UEditorEngine>(GEngine);
		if (EdEngine->GetPlayInEditorSessionInfo().IsSet())
		{
			bIsVRPreview = EdEngine->GetPlayInEditorSessionInfo()->OriginalRequestParams.SessionPreviewTypeOverride ==
				EPlaySessionPreviewType::VRPreview;
		}
	}

	if (bIsVRPreview)
	{
		UHeadMountedDisplayFunctionLibrary::SetSpectatorScreenMode(ESpectatorScreenMode::SingleEye);

		GEngine->bUseFixedFrameRate = true;
		GEngine->FixedFrameRate = 72;

		if (FPICOXRDPManager::IsConnectToServiceSucceed()
			&& FPICOXRDPManager::OnBeginPlayStartStreaming())
		{
			PXR_LOGD(PxrUnreal, "PXR_LivePreview  BeginPlay Succeed!");
		}
		else
		{
			PXR_LOGD(PxrUnreal, "PXR_LivePreview  StartStreaming Failed!Please check if PDC is Launching and restart vr preview again");
		}
	}

#endif
}

void FPICOXRHMDDP::OnEndPlay(FWorldContext& InWorldContext)
{
	if (!GEnableVREditorHacks)
	{
		EnableStereo(false);
	}

	if (bIsVRPreview)
	{
		GEngine->bUseFixedFrameRate = false;
		GEngine->FixedFrameRate = 30;
		if (FPICOXRDPManager::SetHandTrackingEnable(false))
		{
			PXR_LOGD(PxrUnreal, "PXR_LivePreview disable HandTracking Succeed!");
		}
		FPICOXRDPManager::OnEndPlayStopStreaming();
	}
}

const FName FPICOXRHMDDP::SystemName(TEXT("PICOXRPreview"));

FString FPICOXRHMDDP::GetVersionString() const
{
	return FString();
}

bool FPICOXRHMDDP::OnStartGameFrame(FWorldContext& WorldContext)
{
	if (bStereoEnabled != bStereoDesired)
	{
		bStereoEnabled = EnableStereo(bStereoDesired);
	}
	return true;
}

void FPICOXRHMDDP::ResetOrientationAndPosition(float yaw)
{
	ResetOrientation(yaw);
	ResetPosition();
}

void FPICOXRHMDDP::ResetOrientation(float Yaw)
{
	BaseOrientation = FQuat::Identity;
}

void FPICOXRHMDDP::ResetPosition()
{
	BaseOffset = FVector();
}

void FPICOXRHMDDP::SetBaseRotation(const FRotator& BaseRot)
{
	BaseOrientation = BaseRot.Quaternion();
}

FRotator FPICOXRHMDDP::GetBaseRotation() const
{
	return FRotator::ZeroRotator;
}

void FPICOXRHMDDP::SetBaseOrientation(const FQuat& BaseOrient)
{
	BaseOrientation = BaseOrient;
}

FQuat FPICOXRHMDDP::GetBaseOrientation() const
{
	return BaseOrientation;
}

void FPICOXRHMDDP::SetBasePosition(const FVector& BasePosition)
{
	BaseOffset = BasePosition;
}

FVector FPICOXRHMDDP::GetBasePosition() const
{
	return BaseOffset;
}

bool FPICOXRHMDDP::IsStereoEnabled() const
{
	return true;
}

bool FPICOXRHMDDP::EnableStereo(bool bStereo)
{
	if (bStereoEnabled == bStereo)
	{
		return false;
	}

	if ((!bStereo))
	{
		return false;
	}

	bStereoDesired = (IsHMDEnabled()) ? bStereo : false;

	// Set the viewport to match that of the HMD display
	FSceneViewport* SceneVP = FindSceneViewport();
	if (SceneVP)
	{
		TSharedPtr<SWindow> Window = SceneVP->FindWindow();
		if (Window.IsValid() && SceneVP->GetViewportWidget().IsValid())
		{
			// Set MirrorWindow state on the Window
			Window->SetMirrorWindow(bStereo);

			if (bStereo)
			{
				uint32 Width, Height;

				Width = WindowMirrorBoundsWidth;
				Height = WindowMirrorBoundsHeight;

				bStereoEnabled = bStereoDesired;
				SceneVP->SetViewportSize(Width, Height);
			}
			else
			{
				//flush all commands that might call GetStereoProjectionMatrix or other functions that rely on bStereoEnabled 
				FlushRenderingCommands();

				// Note: Setting before resize to ensure we don't try to allocate a new vr rt.
				bStereoEnabled = bStereoDesired;

				FRHIViewport* const ViewportRHI = SceneVP->GetViewportRHI();
				if (ViewportRHI != nullptr)
				{
					ViewportRHI->SetCustomPresent(nullptr);
				}

				FVector2D size = SceneVP->FindWindow()->GetSizeInScreen();
				SceneVP->SetViewportSize(size.X, size.Y);
				Window->SetViewportSizeDrivenByWindow(true);
			}
		}
	}

	// Uncap fps to enable FPS higher than 62
	GEngine->bForceDisableFrameRateSmoothing = bStereoEnabled;

	return bStereoEnabled;
}

void FPICOXRHMDDP::AdjustViewRect(int32 ViewIndex, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const
{
	SizeX = FMath::CeilToInt(IdealRenderTargetSize.X * PixelDensity);
	SizeY = FMath::CeilToInt(IdealRenderTargetSize.Y * PixelDensity);

	SizeX = SizeX / 2;
	if (ViewIndex == 0)
	{
		X += SizeX;
	}
}

bool FPICOXRHMDDP::GetRelativeEyePose(int32 DeviceId, int32 ViewIndex, FQuat& OutOrientation, FVector& OutPosition)
{
	OutOrientation = FQuat::Identity;
	OutPosition = FVector::ZeroVector;
	if (DeviceId == IXRTrackingSystem::HMDDeviceId && (ViewIndex == 0 || ViewIndex == 1))
	{
		OutPosition = FVector(0, (ViewIndex == 0 ? -.5 : .5) * 0.064f * GetWorldToMetersScale(), 0);
		return true;
	}
	else
	{
		return false;
	}
}

void FPICOXRHMDDP::CalculateStereoViewOffset(const int32 ViewIndex, FRotator& ViewRotation, const float WorldToMeters, FVector& ViewLocation)
{
	// Needed to transform world locked stereo layers
	PlayerLocation = ViewLocation;

	// Forward to the base implementation (that in turn will call the DefaultXRCamera implementation)
	FHeadMountedDisplayBase::CalculateStereoViewOffset(ViewIndex, ViewRotation, WorldToMeters, ViewLocation);
}

FMatrix FPICOXRHMDDP::GetStereoProjectionMatrix(const int32 ViewIndex) const
{
	check(IsStereoEnabled() || IsHeadTrackingEnforced());
	const float ProjectionCenterOffset = 0; // 0.151976421f;
	const float PassProjectionOffset = (ViewIndex == 0) ? ProjectionCenterOffset : -ProjectionCenterOffset;
	// correct far and near planes for reversed-Z projection matrix
	const float WorldScale = GetWorldToMetersScale() * (1.0 / 100.0f); // physical scale is 100 UUs/meter
	float ZNear = GNearClippingPlane * WorldScale;

	const float HalfUpFov = FPlatformMath::Tan(BothFrustum.FovUp);
	const float HalfDownFov = FPlatformMath::Tan(BothFrustum.FovDown);
	const float HalfLeftFov = FPlatformMath::Tan(BothFrustum.FovLeft);
	const float HalfRightFov = FPlatformMath::Tan(BothFrustum.FovRight);
	float SumRL = (HalfRightFov + HalfLeftFov);
	float SumTB = (HalfUpFov + HalfDownFov);
	float InvRL = (1.0f / (HalfRightFov - HalfLeftFov));
	float InvTB = (1.0f / (HalfUpFov - HalfDownFov));
	FMatrix ProjectionMatrix = FMatrix(
		FPlane((2.0f * InvRL), 0.0f, 0.0f, 0.0f),
		FPlane(0.0f, (2.0f * InvTB), 0.0f, 0.0f),
		FPlane((SumRL * -InvRL), (SumTB * -InvTB), 0.0f, 1.0f),
		FPlane(0.0f, 0.0f, ZNear, 0.0f)) * FTranslationMatrix(FVector(PassProjectionOffset, 0, 0));
	return ProjectionMatrix;
}

bool FPICOXRHMDDP::GetHMDDistortionEnabled(EShadingPath /* ShadingPath */) const
{
	return false;
}

void FPICOXRHMDDP::OnBeginRendering_GameThread()
{
	check(IsInGameThread());
	SpectatorScreenController->BeginRenderViewFamily();
}

void FPICOXRHMDDP::OnBeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily)
{
	check(IsInRenderingThread());
	//UpdatePoses();

	check(pBridge);
	pBridge->BeginRendering_RenderThread(RHICmdList);

	check(SpectatorScreenController);
	SpectatorScreenController->UpdateSpectatorScreenMode_RenderThread();

	// Update PlayerOrientation used by StereoLayers positioning
	const FSceneView* MainView = ViewFamily.Views[0];
	const FQuat ViewOrientation = MainView->ViewRotation.Quaternion();
	PlayerOrientation = ViewOrientation * MainView->BaseHmdOrientation.Inverse();
}

FXRRenderBridge* FPICOXRHMDDP::GetActiveRenderBridge_GameThread(bool /* bUseSeparateRenderTarget */)
{
	check(IsInGameThread());

	return pBridge;
}

void FPICOXRHMDDP::CalculateRenderTargetSize(const class FViewport& Viewport, uint32& InOutSizeX, uint32& InOutSizeY)
{
	if (!IsStereoEnabled())
	{
		return;
	}

	InOutSizeX = FMath::CeilToInt(IdealRenderTargetSize.X * PixelDensity);
	InOutSizeY = FMath::CeilToInt(IdealRenderTargetSize.Y * PixelDensity);

	check(InOutSizeX != 0 && InOutSizeY != 0);
}

bool FPICOXRHMDDP::NeedReAllocateViewportRenderTarget(const FViewport& Viewport)
{
	check(IsInGameThread());

	if (IsStereoEnabled())
	{
		const uint32 InSizeX = Viewport.GetSizeXY().X;
		const uint32 InSizeY = Viewport.GetSizeXY().Y;
		const FIntPoint RenderTargetSize = Viewport.GetRenderTargetTextureSizeXY();
		uint32 NewSizeX = InSizeX, NewSizeY = InSizeY;
		CalculateRenderTargetSize(Viewport, NewSizeX, NewSizeY);
		if (NewSizeX != RenderTargetSize.X || NewSizeY != RenderTargetSize.Y)
		{
			return true;
		}
	}
	return false;
}

static const uint32 PICODPSwapChainLength = 1;

bool FPICOXRHMDDP::AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, FTextureRHIRef& OutTargetableTexture, FTextureRHIRef& OutShaderResourceTexture, uint32 NumSamples /*= 1*/)
{
	PXR_LOGD(PxrUnreal, "PXR_LivePreview AllocatedRT!");
	if (!IsStereoEnabled())
	{
		return false;
	}

	TArray<FTextureRHIRef> SwapChainTextures;
	FTextureRHIRef BindingTexture;

	if (pBridge != nullptr && pBridge->GetSwapChain() != nullptr && pBridge->GetSwapChain()->GetTexture2D() != nullptr && pBridge->GetSwapChain()->GetTexture2D()->GetSizeX() == SizeX && pBridge->GetSwapChain()->GetTexture2D()->GetSizeY() == SizeY)
	{
		OutTargetableTexture = (FTextureRHIRef&)pBridge->GetSwapChain()->GetTextureRef();
		OutShaderResourceTexture = OutTargetableTexture;
		return true;
	}
	FRHITextureCreateDesc Desc =
		FRHITextureCreateDesc::Create2D(TEXT("FDirectPreviewHMD"))
		.SetExtent(SizeX, SizeY)
		.SetFormat(PF_R8G8B8A8)
		.SetFlags(ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::ShaderResource)
		.SetInitialState(ERHIAccess::SRVMask);

	for (uint32 SwapChainIter = 0; SwapChainIter < PICODPSwapChainLength; ++SwapChainIter)
	{
		FTextureRHIRef TargetableTexture;

		TargetableTexture = RHICreateTexture(Desc);

		SwapChainTextures.Add((FTextureRHIRef&)TargetableTexture);

		if (BindingTexture == nullptr)
		{
			BindingTexture = GDynamicRHI->RHICreateAliasedTexture((FTextureRHIRef&)TargetableTexture);
		}
	}
	TArray<FTextureRHIRef> LeftRHITextureSwapChain = FPICOXRDPManager::GetLeftRHITextureSwapChain();
	TArray<FTextureRHIRef> RightRHITextureSwapChain = FPICOXRDPManager::GetRightRHITextureSwapChain();

	pBridge->CreateLeftSwapChain(FPICOXRDPManager::GetLeftBindingTexture(), MoveTemp(LeftRHITextureSwapChain));
	pBridge->CreateRightSwapChain(FPICOXRDPManager::GetRightBindingTexture(), MoveTemp(RightRHITextureSwapChain));

	pBridge->CreateSwapChain(BindingTexture, MoveTemp(SwapChainTextures));
	// These are the same.
	OutTargetableTexture = (FTextureRHIRef&)BindingTexture;
	OutShaderResourceTexture = (FTextureRHIRef&)BindingTexture;

	return true;
}

FPICOXRHMDDP::FPICOXRHMDDP(const FAutoRegister& AutoRegister, IPICOXRDPModule* InPICODPPlugin) :
	FHeadMountedDisplayBase(nullptr),
	FHMDSceneViewExtension(AutoRegister),
	bHmdEnabled(true),
	HmdWornState(EHMDWornState::Unknown),
	bStereoDesired(false),
	bStereoEnabled(false),
	bOcclusionMeshesBuilt(false),
	WindowMirrorBoundsWidth(2160),
	WindowMirrorBoundsHeight(1200),
	PixelDensity(1.0f),
	HMDWornMovementThreshold(50.0f),
	HMDStartLocation(FVector::ZeroVector),
	BaseOrientation(FQuat::Identity),
	BaseOffset(FVector::ZeroVector),
	bIsQuitting(false),
	QuitTimestamp(),
	bShouldCheckHMDPosition(false),
	RendererModule(nullptr),
	PICODPPlugin(InPICODPPlugin)
{
	Startup();
}

FPICOXRHMDDP::~FPICOXRHMDDP()
{
	Shutdown();
}

bool FPICOXRHMDDP::IsInitialized() const
{
	return true;
}

bool FPICOXRHMDDP::InitializePreviewWindows() const
{
#if WITH_EDITOR
	//user current ScreenResolution as the NewWindowSize
	ULevelEditorPlaySettings* PlaySettingsConfig = GetMutableDefault<ULevelEditorPlaySettings>();
	if (GEngine->IsValidLowLevel())
	{
		const UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
		FIntPoint ScreenResolution = FIntPoint(1080, 1080);
		if (UserSettings->IsValidLowLevel())
		{
			ScreenResolution = UserSettings->GetScreenResolution();
		}
		PlaySettingsConfig->NewWindowHeight = ScreenResolution.Y;
		PlaySettingsConfig->NewWindowWidth = ScreenResolution.Y;
	}
	return true;
#else
	return false;
#endif
}

#define LOCTEXT_NAMESPACE "FPICOXRHMDModule"

void FPICOXRHMDDP::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Engine", "PICOXRLivePreview Settings",
		                                 LOCTEXT("PICOXRLivePreviewSettingsName", "PICOXR LivePreview Settings"),
		                                 LOCTEXT("PICOXRLivePreviewSettingsDescription", "Configure the PICOXRLivePreview plugin"),
		                                 GetMutableDefault<UPICOXRDPSettings>()
		);

		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	}
}


void FPICOXRHMDDP::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Engine", "PICOXRLivePreview Settings");
	}
}
#undef LOCTEXT_NAMESPACE


bool FPICOXRHMDDP::Startup()
{
	RegisterSettings();
	// grab a pointer to the renderer module for displaying our mirror window
	static const FName RendererModuleName("Renderer");
	RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
	PXR_LOGD(PxrUnreal, "PXR_LivePreview startup begin");
	// Re-initialize the plugin if we're canceling the shutdown
	HMDSettings = GetMutableDefault<UPICOXRDPSettings>();
	switch (HMDSettings->GraphicQuality)
	{
	case EGraphicQuality::High:
		{
			IdealRenderTargetSize = FIntPoint(3840, 1920);
		}
		break;
	case EGraphicQuality::Medium:
		{
			IdealRenderTargetSize = FIntPoint(1920, 1920);
		}
		break;
	case EGraphicQuality::Low:
		{
			IdealRenderTargetSize = FIntPoint(1920, 960);
		}
		break;
	default: ;
	}

	//ToDo:Temporary closure of RHI threads has resolved unknown crashes
	bool bEnable=false;
	FString Command = TEXT("r.RHIThread.Enable");
	FString Argument = bEnable ? TEXT("1") : TEXT("0");

	FString FullCommand = Command + TEXT(" ") + Argument;

	if (GEngine)
	{
		GEngine->Exec(nullptr, *FullCommand);
	}

	static const auto PixelDensityCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("vr.PixelDensity"));
	if (PixelDensityCVar)
	{
		PixelDensity = 1;
	}
	// enforce finishcurrentframe
	static IConsoleVariable* CFCFVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.finishcurrentframe"));
	CFCFVar->Set(false);

	{
		pBridge = new D3D11Bridge(this);
		ensure(pBridge != nullptr);
	}

	CreateSpectatorScreenController();

	if (!FPICOXRDPManager::InitializeLivePreview())
	{
		PXR_LOGD(PxrUnreal, "PXR_LivePreview Initialized Failed!");
		return false;
	}

	InitializedSucceeded = true;

	if (!FPICOXRDPManager::ConnectStreamingServer())
	{
		PXR_LOGD(PxrUnreal, "PXR_LivePreview ConnectServer Failed!");
		return false;
	}
	D3D11Device = static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
	SrcBoxRight.left = 0;
	SrcBoxRight.top = 0;
	SrcBoxRight.front = 0;
	SrcBoxRight.right = 3840;
	SrcBoxRight.bottom = 1920;
	SrcBoxRight.back = 1;
	SrcBoxLeft.left = 1920;
	SrcBoxLeft.top = 0;
	SrcBoxLeft.front = 0;
	SrcBoxLeft.right = 3840;
	SrcBoxLeft.bottom = 1920;
	SrcBoxLeft.back = 1;

	D3D11Device->GetImmediateContext(&D3D11DeviceContext);
	
	OnFovUpdatedFromServiceEvent.BindRaw(this, &FPICOXRHMDDP::OnFovStateChanged);
	FPICOXRDPManager::SetFovUpdatedFromServiceEvent(OnFovUpdatedFromServiceEvent);
	PXR_LOGD(PxrUnreal, "PXR_LivePreview start up finished!");
	return true;
}


void FPICOXRHMDDP::Shutdown()
{
	FPICOXRDPManager::ShutDownLivePreview();
	UnregisterSettings();
}

void FPICOXRHMDDP::OnFovStateChanged(const ps_common::DeviceFovInfo& FovInfo)
{
	BothFrustum.FovUp = FovInfo.up();
	BothFrustum.FovDown = FovInfo.down();
	BothFrustum.FovLeft = FovInfo.left();
	BothFrustum.FovRight = FovInfo.right();
	PXR_LOGD(PxrUnreal, "PXR_LivePreview OnFovStateChanged FovUp:%f FovDown:%f FovLeft:%f FovRight:%f!",
	         BothFrustum.FovUp,
	         BothFrustum.FovDown,
	         BothFrustum.FovLeft,
	         BothFrustum.FovRight
	);
}

//necessary, brush the rt on the Spectator screen, which is the window on the PC side
void FPICOXRHMDDP::CreateSpectatorScreenController()
{
	SpectatorScreenController = MakeUnique<FDefaultSpectatorScreenController>(this);
}

FIntRect FPICOXRHMDDP::GetFullFlatEyeRect_RenderThread(FTextureRHIRef EyeTexture) const
{
	static FVector2D SrcNormRectMin(0.05f, 0.2f);
	static FVector2D SrcNormRectMax(0.45f, 0.8f);
	return FIntRect(EyeTexture->GetSizeX() * SrcNormRectMin.X, EyeTexture->GetSizeY() * SrcNormRectMin.Y, EyeTexture->GetSizeX() * SrcNormRectMax.X, EyeTexture->GetSizeY() * SrcNormRectMax.Y);
}

void FPICOXRHMDDP::CopyTexture(FRHICommandList& RHICmdList, FTextureRHIRef SourceTexture, FTextureRHIRef DestTexture, FRHIGPUFence* Fence,bool bLeft,bool bUseRenderPass) const
{
	if (!bUseRenderPass)
	{
		RHICmdList.Transition(FRHITransitionInfo(SourceTexture, ERHIAccess::Unknown, ERHIAccess::CopySrc));
		RHICmdList.Transition(FRHITransitionInfo(DestTexture, ERHIAccess::Unknown, ERHIAccess::CopyDest));
	
		// source and dest are the same. simple copy
		if (bLeft)
		{
			RHICmdList.CopyTexture(SourceTexture, DestTexture, CopyInfoLeft);
		}
		else
		{
			RHICmdList.CopyTexture(SourceTexture, DestTexture, CopyInfoRight);
		}
	}
	else
	{
		IRendererModule* RendererModule1 = &FModuleManager::GetModuleChecked<IRendererModule>("Renderer");

		RHICmdList.Transition(FRHITransitionInfo(SourceTexture, ERHIAccess::Unknown, ERHIAccess::SRVMask));
		RHICmdList.Transition(FRHITransitionInfo(DestTexture, ERHIAccess::Unknown, ERHIAccess::RTV));

		// source and destination are different. rendered copy
		FRHIRenderPassInfo RPInfo(DestTexture, ERenderTargetActions::Load_Store);
		RHICmdList.BeginRenderPass(RPInfo, TEXT("PixelStreaming::CopyTexture"));
		{
			FGlobalShaderMap* ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
			TShaderMapRef<FScreenVS> VertexShader(ShaderMap);
			TShaderMapRef<FScreenPS> PixelShader(ShaderMap);

			RHICmdList.SetViewport(0, 0, 0.0f, DestTexture->GetDesc().Extent.X, DestTexture->GetDesc().Extent.Y, 1.0f);

			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
			GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
			GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
			GraphicsPSOInit.PrimitiveType = PT_TriangleList;
			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

			FRHIBatchedShaderParameters& BatchedParameters = RHICmdList.GetScratchShaderParameters();
			PixelShader->SetParameters(BatchedParameters, TStaticSamplerState<SF_Point>::GetRHI(), SourceTexture);
			RHICmdList.SetBatchedShaderParameters(RHICmdList.GetBoundPixelShader(), BatchedParameters);

			FIntPoint TargetBufferSize(DestTexture->GetDesc().Extent.X, DestTexture->GetDesc().Extent.Y);
			if (bLeft)
			{
				RendererModule1->DrawRectangle(RHICmdList, 0, 0, // Dest X, Y
										 DestTexture->GetDesc().Extent.X,	// Dest Width
										 DestTexture->GetDesc().Extent.Y,	 // Dest Height
										  0.5, 0, // Source U, V
										  0.5, 1, // Source USize, VSize
										  TargetBufferSize, // Target buffer size
										  FIntPoint(1, 1), // Source texture size
										  VertexShader, EDRF_Default);
			}
			else
			{
				RendererModule1->DrawRectangle(RHICmdList, 0, 0, // Dest X, Y
										 DestTexture->GetDesc().Extent.X,	// Dest Width
										 DestTexture->GetDesc().Extent.Y,	// Dest Height
										  0, 0, // Source U, V
										  0.5, 1, // Source USize, VSize
										  TargetBufferSize, // Target buffer size
										  FIntPoint(1, 1), // Source texture size
										  VertexShader, EDRF_Default);
			}
		}

		RHICmdList.EndRenderPass();

		RHICmdList.Transition(FRHITransitionInfo(SourceTexture, ERHIAccess::SRVMask, ERHIAccess::CopySrc));
		RHICmdList.Transition(FRHITransitionInfo(DestTexture, ERHIAccess::RTV, ERHIAccess::CopyDest));
	}

	if (Fence != nullptr)
	{
		RHICmdList.WriteGPUFence(Fence);
	}
}

void FPICOXRHMDDP::CopyTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture* SrcTexture, FIntRect SrcRect, FRHITexture* DstTexture, FIntRect DstRect, bool bClearBlack, bool bNoAlpha) const
{
	check(IsInRenderingThread());

	FRHITexture* LeftTexture2DRHI = pBridge->GetLeftSwapChain()->GetTexture2D();
	FRHITexture* RightTexture2DRHI = pBridge->GetRightSwapChain()->GetTexture2D();

	if (FPICOXRDPManager::IsStreaming())
	{
		if (HMDSettings->GraphicQuality==EGraphicQuality::High)
		{
			FPICOXRDPManager::LockKeyedMutex();
			if (D3D11DeviceContext)
			{
				D3D11DeviceContext->CopySubresourceRegion((ID3D11Resource *)LeftTexture2DRHI->GetNativeResource(), 0, 0, 0, 0, (ID3D11Resource *)SrcTexture->GetNativeResource(), 0, &SrcBoxLeft);
				D3D11DeviceContext->CopySubresourceRegion((ID3D11Resource *)RightTexture2DRHI->GetNativeResource(), 0, 0, 0, 0, (ID3D11Resource *)SrcTexture->GetNativeResource(), 0, &SrcBoxRight);
			}
			FPICOXRDPManager::UnlockKeyedMutex();
		}
		else
		{
			FGPUFenceRHIRef CopyFence = GDynamicRHI->RHICreateGPUFence(*FString::Printf(TEXT("FreezeFrameFence")));
			CopyTexture(RHICmdList, SrcTexture, LeftTexture2DRHI, CopyFence,true,true);
			FGPUFenceRHIRef CopyFence1 = GDynamicRHI->RHICreateGPUFence(*FString::Printf(TEXT("FreezeFrameFence1")));
			CopyTexture(RHICmdList, SrcTexture, RightTexture2DRHI, CopyFence1,false,true);
		}

		uint32 SwapChainIndex = pBridge->GetLeftSwapChain()->GetSwapChainIndex_RHIThread();
		FPICOXRDPManager::SendMessage(SwapChainIndex);
	}

	const uint32 ViewportWidth = DstRect.Width();
	const uint32 ViewportHeight = DstRect.Height();
	const FIntPoint TargetSize(ViewportWidth, ViewportHeight);

	const float SrcTextureWidth = SrcTexture->GetSizeX();
	const float SrcTextureHeight = SrcTexture->GetSizeY();
	float U = 0.f, V = 0.f, USize = 1.f, VSize = 1.f;
	if (!SrcRect.IsEmpty())
	{
		U = SrcRect.Min.X / SrcTextureWidth;
		V = SrcRect.Min.Y / SrcTextureHeight;
		USize = SrcRect.Width() / SrcTextureWidth;
		VSize = SrcRect.Height() / SrcTextureHeight;
	}

	RHICmdList.Transition(FRHITransitionInfo(SrcTexture, ERHIAccess::Unknown, ERHIAccess::SRVGraphics));

	// #todo-renderpasses Possible optimization here - use DontLoad if we will immediately clear the entire target
	FRHIRenderPassInfo RPInfo(DstTexture, ERenderTargetActions::Load_Store);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("CopyTexture"));
	{
		if (bClearBlack)
		{
			const FIntRect ClearRect(0, 0, DstTexture->GetSizeX(), DstTexture->GetSizeY());
			RHICmdList.SetViewport(ClearRect.Min.X, ClearRect.Min.Y, 0, ClearRect.Max.X, ClearRect.Max.Y, 1.0f);
			DrawClearQuad(RHICmdList, FLinearColor::Black);
		}

		RHICmdList.SetViewport(DstRect.Min.X, DstRect.Min.Y, 0, DstRect.Max.X, DstRect.Max.Y, 1.0f);

		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
		GraphicsPSOInit.BlendState = bNoAlpha ? TStaticBlendState<>::GetRHI() : TStaticBlendState<CW_RGBA, BO_Add, BF_SourceAlpha, BF_InverseSourceAlpha, BO_Add, BF_One, BF_InverseSourceAlpha>::GetRHI();
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInit.PrimitiveType = PT_TriangleList;

		const auto FeatureLevel = GMaxRHIFeatureLevel;
		auto ShaderMap = GetGlobalShaderMap(FeatureLevel);

		TShaderMapRef<FScreenVS> VertexShader(ShaderMap);
		GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();

		const bool bSameSize = DstRect.Size() == SrcRect.Size();
		FRHISamplerState* PixelSampler = bSameSize ? TStaticSamplerState<SF_Point>::GetRHI() : TStaticSamplerState<SF_Bilinear>::GetRHI();

		if (EnumHasAnyFlags(SrcTexture->GetFlags(), TexCreate_SRGB))
		{
			TShaderMapRef<FScreenPSsRGBSource> PixelShader(ShaderMap);
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);
			FRHIBatchedShaderParameters& BatchedParameters = RHICmdList.GetScratchShaderParameters();
			PixelShader->SetParameters(BatchedParameters, PixelSampler, SrcTexture);
			RHICmdList.SetBatchedShaderParameters(RHICmdList.GetBoundPixelShader(), BatchedParameters);
		}
		else
		{
			TShaderMapRef<FScreenPS> PixelShader(ShaderMap);
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();

			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);
			FRHIBatchedShaderParameters& BatchedParameters = RHICmdList.GetScratchShaderParameters();
			PixelShader->SetParameters(BatchedParameters, PixelSampler, SrcTexture);
			RHICmdList.SetBatchedShaderParameters(RHICmdList.GetBoundPixelShader(), BatchedParameters);
		}

		RendererModule->DrawRectangle(
			RHICmdList,
			0, 0,
			ViewportWidth, ViewportHeight,
			U, V,
			USize, VSize,
			TargetSize,
			FIntPoint(1, 1),
			VertexShader,
			EDRF_Default);
	}
	RHICmdList.EndRenderPass();
}

void FPICOXRHMDDP::RenderTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture* BackBuffer, FRHITexture* SrcTexture, FVector2D WindowSize) const
{
	check(IsInRenderingThread());

	if (bSplashIsShown || !IsBackgroundLayerVisible())
	{
		FRHIRenderPassInfo RPInfo(SrcTexture, ERenderTargetActions::DontLoad_Store);
		RHICmdList.BeginRenderPass(RPInfo, TEXT("Clear"));
		{
			DrawClearQuad(RHICmdList, FLinearColor(0, 0, 0, 0));
		}
		RHICmdList.EndRenderPass();
	}

	check(SpectatorScreenController);
	SpectatorScreenController->RenderSpectatorScreen_RenderThread(RHICmdList, BackBuffer, SrcTexture, WindowSize);
}

void FPICOXRHMDDP::PostRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView)
{
}

bool FPICOXRHMDDP::IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const
{
	return GEngine && GEngine->IsStereoscopic3D(Context.Viewport);
}


void FPICOXRHMDDP::BridgeBaseImpl::BeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList)
{}

void FPICOXRHMDDP::BridgeBaseImpl::BeginRendering_RHI()
{
	check(!IsRunningRHIInSeparateThread() || IsInRHIThread());
}

void FPICOXRHMDDP::BridgeBaseImpl::CreateSwapChain(const FTextureRHIRef& BindingTexture, TArray<FTextureRHIRef>&& SwapChainTextures)
{
	PXR_LOGD(PxrUnreal, "PXR_LivePreview CreateSwapChain!");
	check(IsInRenderingThread());
	check(SwapChainTextures.Num());
	SwapChain = CreateXRSwapChain(MoveTemp(SwapChainTextures), BindingTexture);
}

void FPICOXRHMDDP::BridgeBaseImpl::CreateLeftSwapChain(const FTextureRHIRef& BindingTexture, TArray<FTextureRHIRef>&& SwapChainTextures)
{
	PXR_LOGD(PxrUnreal, "PXR_LivePreview CreateLeftSwapChain!");
	check(IsInRenderingThread());
	check(SwapChainTextures.Num());
	LeftSwapChain = CreateXRSwapChain(MoveTemp(SwapChainTextures), BindingTexture);
}

void FPICOXRHMDDP::BridgeBaseImpl::CreateRightSwapChain(const FTextureRHIRef& BindingTexture, TArray<FTextureRHIRef>&& SwapChainTextures)
{
	PXR_LOGD(PxrUnreal, "PXR_LivePreview CreateRightSwapChain!");
	check(IsInRenderingThread());
	check(SwapChainTextures.Num());
	RightSwapChain = CreateXRSwapChain(MoveTemp(SwapChainTextures), BindingTexture);
}

bool FPICOXRHMDDP::BridgeBaseImpl::Present(int& SyncInterval)
{
	//This  must return true。
	check(IsRunningRHIInSeparateThread() ? IsInRHIThread() : IsInRenderingThread());

	//necessary, brush the RT to steam
	FinishRendering();
	// Increment swap chain index post-swap.
	SwapChain->IncrementSwapChainIndex_RHIThread();
	if (FPICOXRDPManager::IsStreaming())
	{
		GetLeftSwapChain()->IncrementSwapChainIndex_RHIThread();
		GetRightSwapChain()->IncrementSwapChainIndex_RHIThread();
	}
	

	SyncInterval = 0;

	return true;
}

bool FPICOXRHMDDP::BridgeBaseImpl::NeedsNativePresent()
{
	//This return value does not affect the PC display
	return true;
}

void FPICOXRHMDDP::BridgeBaseImpl::PostPresent()
{
}

FPICOXRHMDDP::D3D11Bridge::D3D11Bridge(FPICOXRHMDDP* plugin)
	: BridgeBaseImpl(plugin)
{
}

//necessary, brush the RT to steam
void FPICOXRHMDDP::D3D11Bridge::FinishRendering()
{
}


void FPICOXRHMDDP::D3D11Bridge::Reset()
{
}

void FPICOXRHMDDP::D3D11Bridge::UpdateViewport(const FViewport& Viewport, FRHIViewport* InViewportRHI)
{
}

#endif // STEAMVR_SUPPORTED_PLATFORMS

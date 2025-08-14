//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

#pragma once
#include "IPXR_DPModule.h"
#include "HardwareInfo.h"

#if 1

#include "HeadMountedDisplay.h"
#include "HeadMountedDisplayBase.h"
#include "IStereoLayers.h"
#include "StereoLayerManager.h"
#include "XRRenderTargetManager.h"
#include "XRRenderBridge.h"
#include "XRSwapChain.h"
#include "IHeadMountedDisplayVulkanExtensions.h"
#include "SceneViewExtension.h"

#include "PXR_DPManager.h"

class IRendererModule;

enum class EEyeType
{
	EyeLeft = 0,
	EyeRight = 1,
	EyeBoth = 2,
};

struct FPICOXRFrustumDP
{
	float FovLeft;
	float FovRight;
	float FovUp;
	float FovDown;
	float Near;
	float Far;
	EEyeType Type;

	FPICOXRFrustumDP()
	{
		FovUp = 0.907f;
		FovDown = -0.907f;
		FovLeft = -0.907f;
		FovRight = 0.907f;
		Near = 0.0508f;
		Far = 100;
		Type=EEyeType::EyeBoth;
	}
	FString ToString() const
	{
		return
			TEXT(" FPICOXRFrustum Left : ") + FString::SanitizeFloat(FovLeft) +
			TEXT(" FPICOXRFrustum Right : ") + FString::SanitizeFloat(FovRight) +
			TEXT(" FPICOXRFrustum Up : ") + FString::SanitizeFloat(FovUp) +
			TEXT(" FPICOXRFrustum Down : ") + FString::SanitizeFloat(FovDown) +
			TEXT(" FPICOXRFrustum H : ") + FString::SanitizeFloat(FovRight - FovLeft) +
			TEXT(" FPICOXRFrustum V : ") + FString::SanitizeFloat(FovUp - FovDown) +
			TEXT(" FPICOXRFrustum Near : ") + FString::SanitizeFloat(Near) +
			TEXT(" FPICOXRFrustum Far : ") + FString::SanitizeFloat(Far);
	}
};

/** Stores vectors, in clockwise order, to define soft and hard bounds for Chaperone */
struct FBoundingQuad
{
	FVector Corners[4];
};

/**
 * Struct for managing stereo layer data.
 */
struct FPICODPLayer
{
	typedef IStereoLayers::FLayerDesc FLayerDesc;
	FLayerDesc	          LayerDesc;
	bool				  bUpdateTexture;

	FPICODPLayer(const FLayerDesc& InLayerDesc)
		: LayerDesc(InLayerDesc)
		, bUpdateTexture(false)
	{}

	// Required by TStereoLayerManager:
	void SetLayerId(uint32 InId) { LayerDesc.SetLayerId(InId); }
	uint32 GetLayerId() const { return LayerDesc.GetLayerId(); }
	friend bool GetLayerDescMember(const FPICODPLayer& Layer, FLayerDesc& OutLayerDesc);
	friend void SetLayerDescMember(FPICODPLayer& Layer, const FLayerDesc& InLayerDesc);
	friend void MarkLayerTextureForUpdate(FPICODPLayer& Layer);
};

/**
 * PICODP Head Mounted Display public FPICODPAssetManager,
 */
class FPICOXRHMDDP : public FHeadMountedDisplayBase, public FXRRenderTargetManager, public TStereoLayerManager<FPICODPLayer>, public FHMDSceneViewExtension
{
public:
	/** Constructor */
	FPICOXRHMDDP(const FAutoRegister&, IPICOXRDPModule*);

	/** Destructor */
	virtual ~FPICOXRHMDDP();

	/** @return	True if the API was initialized OK */
	bool IsInitialized() const;

	PICOXRDPHMD_API bool InitializePreviewWindows() const;
	
	static const FName SystemName;
	/** IXRTrackingSystem interface */
	virtual FName GetSystemName() const override
	{
		return SystemName;
	}
	virtual int32 GetXRSystemFlags() const override
	{
		return EXRSystemFlags::IsHeadMounted;
	}
	virtual FString GetVersionString() const override;

	virtual class IHeadMountedDisplay* GetHMDDevice() override
	{
		return this;
	}

	virtual class TSharedPtr< class IStereoRendering, ESPMode::ThreadSafe > GetStereoRenderingDevice() override
	{
		return SharedThis(this);
	}

	virtual bool OnStartGameFrame(FWorldContext& WorldContext) override;
	virtual bool DoesSupportPositionalTracking() const override;
	virtual bool HasValidTrackingPosition() override;
	virtual bool EnumerateTrackedDevices(TArray<int32>& TrackedIds, EXRTrackedDeviceType DeviceType = EXRTrackedDeviceType::Any) override;

	virtual bool GetTrackingSensorProperties(int32 InDeviceId, FQuat& OutOrientation, FVector& OutOrigin, FXRSensorProperties& OutSensorProperties) override;
	virtual FString GetTrackedDevicePropertySerialNumber(int32 DeviceId) override;
	virtual bool GetCurrentPose(int32 DeviceId, FQuat& CurrentOrientation, FVector& CurrentPosition) override;
	virtual bool GetRelativeEyePose(int32 DeviceId,int32 ViewIndex, FQuat& OutOrientation, FVector& OutPosition) override;
	virtual bool IsTracking(int32 DeviceId) override;

	virtual void ResetOrientationAndPosition(float yaw = 0.f) override;
	virtual void ResetOrientation(float Yaw = 0.f) override;
	virtual void ResetPosition() override;

	virtual void SetBaseRotation(const FRotator& BaseRot) override;
	virtual FRotator GetBaseRotation() const override;
	virtual void SetBaseOrientation(const FQuat& BaseOrient) override;
	virtual FQuat GetBaseOrientation() const override;
	virtual void SetBasePosition(const FVector& BasePosition) override;
	virtual FVector GetBasePosition() const override;
	virtual void OnBeginPlay(FWorldContext& InWorldContext) override;
	virtual void OnEndPlay(FWorldContext& InWorldContext) override;
	virtual void RecordAnalytics() override;

	virtual void SetTrackingOrigin(EHMDTrackingOrigin::Type NewOrigin) override;
	virtual EHMDTrackingOrigin::Type GetTrackingOrigin() const override;
	virtual bool GetFloorToEyeTrackingTransform(FTransform& OutFloorToEye) const override;
	virtual FVector2D GetPlayAreaBounds(EHMDTrackingOrigin::Type Origin) const override;
	void CopyTexture(FRHICommandList& RHICmdList, FTextureRHIRef SourceTexture, FTextureRHIRef DestTexture, FRHIGPUFence* Fence,bool bLeft,bool bUseRenderPass) const;

public:
	/** IHeadMountedDisplay interface */
	virtual bool IsHMDConnected() override;
	virtual bool IsHMDEnabled() const override;
	virtual EHMDWornState::Type GetHMDWornState() override;
	virtual void EnableHMD(bool allow = true) override;
	virtual bool GetHMDMonitorInfo(MonitorInfo&) override;

	virtual void GetFieldOfView(float& OutHFOVInDegrees, float& OutVFOVInDegrees) const override;

	virtual void SetInterpupillaryDistance(float NewInterpupillaryDistance) override;
	virtual float GetInterpupillaryDistance() const override;

	virtual bool IsChromaAbCorrectionEnabled() const override;

	virtual void UpdateScreenSettings(const FViewport* InViewport) override {}

	virtual bool GetHMDDistortionEnabled(EShadingPath ShadingPath) const override;

	virtual void OnBeginRendering_GameThread() override;
	virtual void OnBeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& ViewFamily) override;

	virtual float GetPixelDenity() const override { return 1; }
	virtual void SetPixelDensity(const float NewDensity) override { PixelDensity = NewDensity; }
	virtual FIntPoint GetIdealRenderTargetSize() const override { return IdealRenderTargetSize; }

	//PICO Preview++++++++++++++++++++++
	/** IStereoRendering interface */
	virtual bool IsStereoEnabled() const override;
	virtual bool EnableStereo(bool stereo = true) override;
	virtual void AdjustViewRect(int32 ViewIndex, int32& X, int32& Y, uint32& SizeX, uint32& SizeY) const override;
	virtual void CalculateStereoViewOffset(const int32 ViewIndex, FRotator& ViewRotation, const float MetersToWorld, FVector& ViewLocation) override;
	virtual FMatrix GetStereoProjectionMatrix(const int32 ViewIndex) const override;
	virtual void RenderTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture* BackBuffer, FRHITexture* SrcTexture, FVector2D WindowSize) const override;
	//virtual void GetEyeRenderParams_RenderThread(const FRenderingCompositePassContext& Context, FVector2D& EyeToSrcUVScaleValue, FVector2D& EyeToSrcUVOffsetValue) const override;
	virtual IStereoRenderTargetManager* GetRenderTargetManager() override { return this; }

	/** FXRRenderTargetManager interface */
	virtual FXRRenderBridge* GetActiveRenderBridge_GameThread(bool bUseSeparateRenderTarget) override;
	virtual bool ShouldUseSeparateRenderTarget() const override
	{
		check(IsInGameThread());
		return IsStereoEnabled();
	}
	virtual void CalculateRenderTargetSize(const class FViewport& Viewport, uint32& InOutSizeX, uint32& InOutSizeY) override;
	virtual bool NeedReAllocateViewportRenderTarget(const class FViewport& Viewport) override;
	//virtual bool AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags InTexFlags, ETextureCreateFlags InTargetableTextureFlags, FTexture2DRHIRef& OutTargetableTexture, FTexture2DRHIRef& OutShaderResourceTexture, uint32 NumSamples = 1) override;
	virtual bool AllocateRenderTargetTexture(uint32 Index, uint32 SizeX, uint32 SizeY, uint8 Format, uint32 NumMips, ETextureCreateFlags Flags, ETextureCreateFlags TargetableTextureFlags, FTextureRHIRef& OutTargetableTexture, FTextureRHIRef& OutShaderResourceTexture, uint32 NumSamples = 1) override;
	virtual bool ShouldCopyDebugLayersToSpectatorScreen() const override { return true; }
	//PICO Preview-----------------------
	//ISceneViewExtension interface
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {};
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void PreRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override {}
	virtual void PreRenderViewFamily_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneViewFamily& InViewFamily) override {};
	virtual void PostRenderView_RenderThread(FRHICommandListImmediate& RHICmdList, FSceneView& InView) override;
	//PICO Preview ！！！
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override;
	//FPICOXRDPManagerPtr CurrentDPManager = NULL;
	// SpectatorScreen
	//PICO Preview++++++++++++++++++++++++
private:
	void CreateSpectatorScreenController();
public:
	virtual FIntRect GetFullFlatEyeRect_RenderThread(FTextureRHIRef EyeTexture) const override;
	virtual void CopyTexture_RenderThread(FRHICommandListImmediate& RHICmdList, FRHITexture* SrcTexture, FIntRect SrcRect, FRHITexture* DstTexture, FIntRect DstRect, bool bClearBlack, bool bNoAlpha) const override;
	//PICO Preview-------------------
	class BridgeBaseImpl : public FXRRenderBridge
	{
	public:
		BridgeBaseImpl(FPICOXRHMDDP* plugin)
			: Plugin(plugin)
			, bInitialized(false)
			, bUseExplicitTimingMode(false)
		{}

		// Render bridge virtual interface
		virtual bool Present(int& SyncInterval) override;
		virtual void PostPresent() override;
		virtual bool NeedsNativePresent() override;

		// Non-virtual public interface
		bool IsInitialized() const { return bInitialized; }

		bool IsUsingExplicitTimingMode() const
		{
			return bUseExplicitTimingMode;
		}

		FXRSwapChainPtr GetSwapChain() { return SwapChain; }
		FXRSwapChainPtr GetLeftSwapChain() { return LeftSwapChain; }
		FXRSwapChainPtr GetRightSwapChain() { return RightSwapChain; }
		
		FXRSwapChainPtr GetDepthSwapChain() { return DepthSwapChain; }

		/** Schedules BeginRendering_RHI on the RHI thread when in explicit timing mode */
		void BeginRendering_RenderThread(FRHICommandListImmediate& RHICmdList);

		/** Called only when we're in explicit timing mode, which needs to be paired with a call to PostPresentHandoff */
		void BeginRendering_RHI();

		void CreateSwapChain(const FTextureRHIRef& BindingTexture, TArray<FTextureRHIRef>&& SwapChainTextures);
		void CreateLeftSwapChain(const FTextureRHIRef& BindingTexture, TArray<FTextureRHIRef>&& SwapChainTextures);
		void CreateRightSwapChain(const FTextureRHIRef& BindingTexture, TArray<FTextureRHIRef>&& SwapChainTextures);

		// Virtual interface implemented by subclasses
		virtual void Reset() = 0;

	private:
		virtual void FinishRendering() = 0;

	protected:
		
		FPICOXRHMDDP* Plugin;
		FXRSwapChainPtr			SwapChain;
		FXRSwapChainPtr			LeftSwapChain;
		FXRSwapChainPtr			RightSwapChain;

		FXRSwapChainPtr			DepthSwapChain;

		bool					bInitialized;

		/** If we use explicit timing mode, we must have matching calls to BeginRendering_RHI and PostPresentHandoff */
		bool					bUseExplicitTimingMode;

	};

#if PLATFORM_WINDOWS
	class D3D11Bridge : public BridgeBaseImpl
	{
	public:
		D3D11Bridge(FPICOXRHMDDP* plugin);
		/// <summary>
		/// Brush the RT to the runtime of steam
		virtual void FinishRendering() override;

		virtual void UpdateViewport(const FViewport& Viewport, FRHIViewport* InViewportRHI) override;
		virtual void Reset() override;
		TRefCountPtr<ID3D11Texture2D> intexture;

	};
#endif // PLATFORM_WINDOWS
protected:

	virtual float GetWorldToMetersScale() const override;


private:

	/**
	 * Starts up the OpenVR API. Returns true if initialization was successful, false if not.
	 */
	bool Startup();

	/**
	 * Shuts down the OpenVR API
	 */
	void Shutdown();

	void RegisterSettings();
	void UnregisterSettings();


private:
	class UPICOXRDPSettings* HMDSettings;
	FPICOXRFrustumDP BothFrustum;

	FRHICopyTextureInfo CopyInfoLeft;
	FRHICopyTextureInfo CopyInfoRight;
	ID3D11Device*  D3D11Device  = nullptr;
	ID3D11DeviceContext* D3D11DeviceContext = nullptr;
	D3D11_BOX SrcBoxLeft;
	D3D11_BOX SrcBoxRight;
	
	bool bIsVRPreview;
	bool InitializedSucceeded;
	bool bHmdEnabled;
	EHMDWornState::Type HmdWornState;
	bool bStereoDesired;
	bool bStereoEnabled;
	bool bOcclusionMeshesBuilt;
	FOnFovUpdatedFromServiceEvent OnFovUpdatedFromServiceEvent;

	void OnFovStateChanged(const ps_common::DeviceFovInfo& FovInfo);

	// Current world to meters scale. Should only be used when refreshing poses.
	// Everywhere else, use the current tracking frame's WorldToMetersScale.
	float GameWorldToMetersScale;

	FHMDViewMesh HiddenAreaMeshes[2];
	FHMDViewMesh VisibleAreaMeshes[2];



	uint32 WindowMirrorBoundsWidth;
	uint32 WindowMirrorBoundsHeight;

	FIntPoint IdealRenderTargetSize;
	float PixelDensity;

	/** How far the HMD has to move before it's considered to be worn */
	float HMDWornMovementThreshold;

	/** used to check how much the HMD has moved for changing the Worn status */
	FVector					HMDStartLocation;

	// HMD base values, specify forward orientation and zero pos offset
	FQuat					BaseOrientation;	// base orientation
	FVector					BaseOffset;

	// State for tracking quit operation
	bool					bIsQuitting;
	double					QuitTimestamp;

	/**  True if the HMD sends an event that the HMD is being interacted with */
	bool					bShouldCheckHMDPosition;

	IRendererModule* RendererModule;
	IPICOXRDPModule* PICODPPlugin;

	FString DisplayId;

	FQuat PlayerOrientation;
	FVector PlayerLocation;

	TRefCountPtr<BridgeBaseImpl> pBridge;
};

#endif //STEAMVR_SUPPORTED_PLATFORMS

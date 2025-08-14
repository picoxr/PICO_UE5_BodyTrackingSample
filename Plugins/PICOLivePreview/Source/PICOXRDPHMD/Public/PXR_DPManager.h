//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#if PLATFORM_WINDOWS
#include "D3D11RHIPrivate.h"
#include "ps_driver_sdk/streaming_driver_interface.h"
#endif
using namespace ps_driver_sdk;

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Microsoft/COMPointer.h"
#include "Windows/HideWindowsPlatformTypes.h"
DECLARE_DELEGATE_TwoParams(FOnDeviceConnectedResultsEvent, const DeviceInfo&, int32);
DECLARE_DELEGATE_OneParam(FOnConnectFromServiceEvent, int32);
DECLARE_DELEGATE_OneParam(FOnFovUpdatedFromServiceEvent, const ps_common::DeviceFovInfo&);

UENUM(BlueprintType)
enum class EEyeSide : uint8
{
	Left,
	Right,
	Both,
};


class StreamingDataReceiveWrapper : public StreamingDriverInterfaceCallback
{
public:
	StreamingDataReceiveWrapper() = default;
	virtual void NotifyFovUpdated(const ps_common::DeviceFovInfo& fov_info) override;
	virtual void NotifyIPDUpdated(float ipd) override;
	virtual void NotifyDevicePoseUpdated(const ps_common::DevicePose pose_arr[], size_t arr_size) override;
	virtual void NotifyControllerButtonUpdated(const ps_common::ControllerButton& button) override;
	virtual void NotifyEyeTrackingDataUpdated(const ps_common::EyeTrackingData& et_data) override;
	virtual void NotifyConnectToService(ResultCode result) override;
	virtual void NotifyDisconnectFromService(ResultCode reason) override;

	virtual void NotifyDeviceConnected(const DeviceInfo& device_info) override;
	virtual void NotifyDeviceDisconnected(const DeviceInfo& device_info) override;

	virtual void NotifyStreamingStateUpdated(StreamingState state) override;
	virtual void NotifyControllerStateUpdated(const ps_common::ControllerState controller_states[], size_t size) override;
	virtual void NotifyHandJointUpdated(const ps_common::HandJointLocations hand_joints[], size_t arr_size) override;

	virtual void NotifyBodyJointsUpdated(const ps_common::BodyJointLocations& body_joints) override;
	virtual void NotifyBodyTrackingState(int32_t status_code, int32_t error_code) override;

	ps_common::Quaternion GetControllerRotation(EControllerHand DeviceHand) const;
	ps_common::Vector GetControllerPosition(EControllerHand DeviceHand) const;

	ps_common::Vector GetHMDPosition() const;
	ps_common::Quaternion GetHMDRotation() const;

	bool GetDeviceFovInfo(ps_common::DeviceFovInfo& FovInfo) const;
	float GetJoystickX(EControllerHand DeviceHand) const;
	float GetJoystickY(EControllerHand DeviceHand) const;
	float GetTriggerValue(EControllerHand DeviceHand) const;
	float GetGripValue(EControllerHand DeviceHand) const;
	float GetStateValue(EControllerHand DeviceHand) const;
	bool GetConnectionStatus(EControllerHand DeviceHand) const;
	bool IsStreaming() const;
	bool GetStreamingConfig(StreamingConfig& streamingConfig) const;

	bool IsHandTrackingActived(EControllerHand DeviceHand) const;
	bool IsEyeTrackingActived(EEyeSide Side) const;

	uint32 GetHandJointCount(EControllerHand DeviceHand) const;
	TArray<ps_common::HandJointLocation> GetHandJointInfos(EControllerHand DeviceHand) const;

	ps_common::HandAimState GetHandAimState(EControllerHand DeviceHand) const;
	ps_common::EyeTrackingDataPICO GetEyeTrackingDataPICO() const;

	ps_common::BodyJointLocations GetBodyJointLocations() const;

	FOnDeviceConnectedResultsEvent OnDeviceConnectedResultsEvent;
	FOnConnectFromServiceEvent OnConnectFromServiceEvent;
	FOnFovUpdatedFromServiceEvent OnFovUpdatedFromServiceEvent;
	FCriticalSection StateLock;

private:
	//DeviceInfo Device_Info;
	bool bLeftControllerActive = false;
	bool bRightControllerActive = false;
	
	uint8 LeftControllerBattery = 0;
	uint8 RightControllerBattery = 0;

	bool bStreaming = false;
	ps_common::DeviceFovInfo PDCDeviceFovInfo = {};
	StreamingConfig PDCStreamingConfig = {};

	ps_common::Vector LeftPosition = {};
	ps_common::Quaternion LeftRotation = {};
	ps_common::Vector RightPosition = {};
	ps_common::Quaternion RightRotation = {};
	ps_common::Vector HMDPosition = {};
	ps_common::Quaternion HMDRotation = {};
	bool bEyeTrackingDataValid = false;
	ps_common::EyeTrackingDataPICO EyeTrackingData = {};

	float LeftJoystickX = 0.f;
	float LeftJoystickY = 0.f;
	float LeftTriggerValue = 0.f;
	float LeftGripValue = 0.f;
	int32 LeftState = 0;

	float RightJoystickX = 0.f;
	float RightJoystickY = 0.f;
	float RightTriggerValue = 0.f;
	float RightGripValue = 0.f;
	int32 RightState = 0;

	bool RightHand_is_active = false;
	uint32 RightHand_joint_count = 0;
	TArray<ps_common::HandJointLocation> RightHand_joint_infos;

	bool LeftHand_is_active = false;
	uint32 LeftHand_joint_count = 0;
	TArray<ps_common::HandJointLocation> LeftHand_joint_infos;

	ps_common::HandAimState LeftHandAimState = {};
	ps_common::HandAimState RightHandAimState = {};
	
	ps_common::BodyJointLocations BodyLocations = {};
};

struct FPICOXRFrustumDP;

class PICOXRDPHMD_API FPICOXRDPManager
{
public:
	FPICOXRDPManager() = default;
	~FPICOXRDPManager() = default;
	static bool IsStreaming();
	static bool OnBeginPlayStartStreaming();
	static bool OnEndPlayStopStreaming();
	static bool InitializeLivePreview();
	static void ShutDownLivePreview();
	static bool SendMessage(int SwapChainIndex);
	static void GetHMDPositionAndRotation(FVector& OutPostion, FQuat& OutQuat);
	static void GetControllerPositionAndRotation(EControllerHand DeviceHand, float WorldScale, FVector& OutPostion, FRotator& OutQuat);
	static bool GetControllerConnectionStatus(EControllerHand DeviceHand);
	static int32 GetControllerButtonStatus(EControllerHand DeviceHand);
	static void GetControllerAxisValue(EControllerHand DeviceHand, double& JoyStickX, double& JoyStickY, float& TriggerValue, float& GripValue);
	static bool IsConnectToServiceSucceed();
	static bool GetPICOXRFrustumDP(FPICOXRFrustumDP& PICOXRFrustumDP);
	static bool GetHandTrackerJointLocations(EControllerHand DeviceHand, ps_common::HandJointLocations& JointInfos);
	static bool GetHandTrackerAimState(EControllerHand DeviceHand, ps_common::HandAimState& AimState);
	static bool GetEyeTrackingData(ps_common::EyeTrackingDataPICO& EyeTrackingData);

	static bool GetHandTrackerSettingState(bool& State);
	static bool StartEyeTrackingSucceed(EEyeSide DeviceEye);
	static bool ConnectStreamingServer();
	static bool DisConnectStreamingServer();
	static bool SetDisconnectFromServiceEvent(const FOnConnectFromServiceEvent& OnConnectFromServiceEvent);
	static bool SetFovUpdatedFromServiceEvent(const FOnFovUpdatedFromServiceEvent& OnFovUpdatedFromServiceEvent);

	static bool SetHandTrackingEnable(bool Enable);
	static bool SetEyeTrackingEnable(bool Enable);
	static bool SetBodyTrackingEnable(bool Enable);

	//Swift 1.0
	static bool SetSwiftMode(int32 Mode);
	// static bool SetBodyTrackingBoneLength(ps_common::BodyTrackingBoneLength& BoneLength);
	// static bool GetBodyTrackingPose(ps_common::BodyJointLocations& body_joints);
	static bool StartBodyTrackingCalibApp(const char* calib_flag_string,int calib_mode);

	static FTextureRHIRef GetLeftBindingTexture();
	static FTextureRHIRef GetRightBindingTexture();
	
	static TArray<FTextureRHIRef> GetLeftRHITextureSwapChain();
	static TArray<FTextureRHIRef> GetRightRHITextureSwapChain();


	static TComPtr<IDXGIKeyedMutex> GetLeftKeyedMutex();
	static TComPtr<IDXGIKeyedMutex> GetRightKeyedMutex();

	static bool LockKeyedMutex(UINT64 Key =0,DWORD TimeOut_ms = 10);
	static void UnlockKeyedMutex(UINT64 Key =0);

	
private:
	static void OnDeviceConnectionChanged(const DeviceInfo& device_info, int32 State);

	static bool CreateStreamingInterface();
	static bool CreateSharedTexture2D();
	static bool CreateRHITexture(ID3D11Texture2D* OpenedSharedResource, EPixelFormat Format, FTextureRHIRef& OutTexture);

	static bool bHandTrackingStreamingEnabled;
	static bool bEyeTrackingStreamingEnabled;
	static bool bBodyTrackingStreamingEnabled;

	static FTextureRHIRef LeftBindingTexture;
	static FTextureRHIRef RightBindingTexture;

	static TArray<FTextureRHIRef> LeftRHITextureSwapChain;
	static TArray<FTextureRHIRef> RightRHITextureSwapChain;
	
	static bool bConnectToServiceSucceed;
	static StreamingDriverInterface::Ptr_t StreamingDriverInterfacePtr;
	static TSharedPtr<StreamingDataReceiveWrapper, ESPMode::ThreadSafe> StreamingDataWrapperPtr;

	static uint32 SwapChainIndex_RHIThread;
	
	static TArray<TRefCountPtr<ID3D11Texture2D>> LeftDstSwapChainTextures;
	static TArray<TRefCountPtr<ID3D11Texture2D>> RightDstSwapChainTextures;

	static TArray<void*> LeftDstTextureHandles;
	static TArray<void*> RightDstTextureHandles;

	static FTextureRHIRef LeftRHITexture;
	static FTextureRHIRef RightRHITexture;

	static StreamingConfig streamingConfig;
	static TComPtr<IDXGIKeyedMutex> LeftKeyedMutex;
	static TComPtr<IDXGIKeyedMutex> RightKeyedMutex;

	static int32 DefaultBodyJointSet;
	static ps_common::BodyTrackingBoneLength DefaultBoneLength;
};

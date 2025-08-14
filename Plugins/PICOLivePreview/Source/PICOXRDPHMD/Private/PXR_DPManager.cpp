//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.


#include "PXR_DPManager.h"
#include <vector>
#include "ClearQuad.h"
#include "CommonRenderResources.h"
#include "D3D11RHIPrivate.h"
#include "GlobalShader.h"
#include "PXR_Log.h"
#include "ScreenRendering.h"
#include "PXR_DPHMD.h"
#include "PXR_DPSettings.h"
#include "Microsoft/COMPointer.h"

/** Utility struct, similar to FScopeLock but allows the lock to be enabled/disabled more easily */
struct FConditionalScopeLock
{
	UE_NONCOPYABLE(FConditionalScopeLock);

	FConditionalScopeLock(FCriticalSection* InSyncObject, bool bShouldLock)
	{
		checkf(InSyncObject != nullptr, TEXT("InSyncObject must point to a valid FCriticalSection"));

		if (bShouldLock)
		{		
			SyncObject = InSyncObject;
			SyncObject->Lock();
		}
		else
		{
			SyncObject = nullptr;
		}
	}

	/** Destructor that performs a release on the synchronization object. */
	~FConditionalScopeLock()
	{
		if (SyncObject != nullptr)
		{
			SyncObject->Unlock();
		}
	}

private:
	FCriticalSection* SyncObject;
};

static constexpr int32_t MaxHandJointCount = 32;

void StreamingDataReceiveWrapper::NotifyFovUpdated(const ps_common::DeviceFovInfo& fov_info)
{
	PDCDeviceFovInfo = fov_info;
	PXR_LOGD(PxrUnreal, "PXR_LivePreview NotifyFovUpdated");
	OnFovUpdatedFromServiceEvent.ExecuteIfBound(fov_info);
}

void StreamingDataReceiveWrapper::NotifyIPDUpdated(float ipd)
{
	PXR_LOGV(PxrUnreal, "PXR_LivePreview StreamingDataReceiver NotifyIPDUpdated:%f", ipd);
}

ps_common::Quaternion StreamingDataReceiveWrapper::GetControllerRotation(EControllerHand DeviceHand) const
{
	return (DeviceHand == EControllerHand::Left) ? LeftRotation : RightRotation;
}

ps_common::Vector StreamingDataReceiveWrapper::GetHMDPosition() const
{
	return HMDPosition;
}

ps_common::Quaternion StreamingDataReceiveWrapper::GetHMDRotation() const
{
	return HMDRotation;
}

bool StreamingDataReceiveWrapper::GetDeviceFovInfo(ps_common::DeviceFovInfo& FovInfo) const
{
	if (PDCDeviceFovInfo.down()
		&& PDCDeviceFovInfo.up()
		&& PDCDeviceFovInfo.left()
		&& PDCDeviceFovInfo.right())
	{
		FovInfo = PDCDeviceFovInfo;
		return true;
	}

	return false;
}

ps_common::Vector StreamingDataReceiveWrapper::GetControllerPosition(EControllerHand DeviceHand) const
{
	return (DeviceHand == EControllerHand::Left) ? LeftPosition : RightPosition;
}

void StreamingDataReceiveWrapper::NotifyDevicePoseUpdated(const ps_common::DevicePose pose_arr[], size_t arr_size)
{
	for (int index = 0; index < arr_size; index++)
	{
		switch (pose_arr[index].device_type())
		{
		case ps_common::DeviceType::kUndefined: break;
		case ps_common::DeviceType::kHead:
			{
				HMDPosition = pose_arr[index].position();
				HMDRotation = pose_arr[index].rotation();
			}
			break;
		case ps_common::DeviceType::kHandController:
			{
				if (pose_arr[index].side_type() == ps_common::SideType::kLeft)
				{
					LeftPosition = pose_arr[index].position();
					LeftRotation = pose_arr[index].rotation();
					bLeftControllerActive = pose_arr[index].active();
				}
				else
				{
					RightPosition = pose_arr[index].position();
					RightRotation = pose_arr[index].rotation();
					bRightControllerActive = pose_arr[index].active();
				}
			}
			break;
		default: ;
		}
	}
}

void StreamingDataReceiveWrapper::NotifyControllerButtonUpdated(const ps_common::ControllerButton& button)
{
	if (button.side_type() == ps_common::SideType::kLeft)
	{
		LeftJoystickX = button.joystick_x();
		LeftJoystickY = button.joystick_y();
		LeftTriggerValue = button.trigger_value();
		LeftGripValue = button.grip_value();
		LeftState = button.state();
	}
	else
	{
		RightJoystickX = button.joystick_x();
		RightJoystickY = button.joystick_y();
		RightTriggerValue = button.trigger_value();
		RightGripValue = button.grip_value();
		RightState = button.state();
	}
}

void StreamingDataReceiveWrapper::NotifyEyeTrackingDataUpdated(const ps_common::EyeTrackingData& et_data)
{
}

void StreamingDataReceiveWrapper::NotifyConnectToService(ResultCode result)
{
	PXR_LOGD(PxrUnreal, "PXR_LivePreview NotifyConnectToService reason:%d", result);
	OnConnectFromServiceEvent.ExecuteIfBound(1);
}

void StreamingDataReceiveWrapper::NotifyDisconnectFromService(ResultCode reason)
{
	PXR_LOGD(PxrUnreal, "PXR_LivePreview NotifyDisconnectFromService reason:%d", reason);
	OnConnectFromServiceEvent.ExecuteIfBound(0);
}

void StreamingDataReceiveWrapper::NotifyDeviceConnected(const DeviceInfo& device_info)
{
	PXR_LOGD(PxrUnreal, "PXR_LivePreview DeviceName:%s", UTF8_TO_TCHAR(device_info.device_name));
	OnDeviceConnectedResultsEvent.ExecuteIfBound(device_info, 1);
}

void StreamingDataReceiveWrapper::NotifyDeviceDisconnected(const DeviceInfo& device_info)
{
	OnDeviceConnectedResultsEvent.ExecuteIfBound(device_info, 0);
}

void StreamingDataReceiveWrapper::NotifyStreamingStateUpdated(StreamingState state)
{
	bStreaming = (state == StreamingState::kStreamingReady) ? true : false;
	PXR_LOGD(PxrUnreal, "PXR_LivePreview NotifyStreamimgStateUpdated state：%d", bStreaming);

	if (bStreaming)
	{
		if (FPICOXRDPManager::SetHandTrackingEnable(true))
		{
			PXR_LOGD(PxrUnreal, "PXR_LivePreview enable HandTracking Succeed!");
		}
	}
}

void StreamingDataReceiveWrapper::NotifyControllerStateUpdated(const ps_common::ControllerState controller_states[], size_t size)
{
	for (int index = 0; index < size; index++)
	{
		switch (controller_states[index].side_type())
		{
		case ps_common::SideType::kUndefined:
			break;
		case ps_common::SideType::kLeft:
			{
				bLeftControllerActive=controller_states[index].active();
				LeftControllerBattery=controller_states[index].battery_percent();
			}
			break;
		case ps_common::SideType::kRight:
			{
				bRightControllerActive=controller_states[index].active();
				RightControllerBattery=controller_states[index].battery_percent();
			}
			break;
		case ps_common::SideType::kBoth:
			break;
		default: ;
		}
	}
}

void StreamingDataReceiveWrapper::NotifyHandJointUpdated(const ps_common::HandJointLocations hand_joints[], size_t arr_size)
{
	LeftHand_joint_infos.Init(ps_common::HandJointLocation(), MaxHandJointCount);
	RightHand_joint_infos.Init(ps_common::HandJointLocation(), MaxHandJointCount);

	for (int index = 0; index < arr_size; index++)
	{
		switch (hand_joints[index].side_type)
		{
		case ps_common::SideType::kUndefined:
			break;
		case ps_common::SideType::kLeft:
			{
				LeftHand_is_active = hand_joints[index].is_active;
				if (hand_joints[index].is_active)
				{
					LeftHand_joint_count = hand_joints[index].joint_count;
					FMemory::Memcpy(LeftHand_joint_infos.GetData(), hand_joints[index].joint_locations, sizeof(ps_common::HandJointLocation) * MaxHandJointCount);
					FMemory::Memcpy(&LeftHandAimState, &hand_joints[index].aim_state, sizeof(ps_common::HandAimState));
				}
			}
			break;
		case ps_common::SideType::kRight:
			{
				RightHand_is_active = hand_joints[index].is_active;
				if (hand_joints[index].is_active)
				{
					RightHand_joint_count = hand_joints[index].joint_count;
					FMemory::Memcpy(RightHand_joint_infos.GetData(), hand_joints[index].joint_locations, sizeof(ps_common::HandJointLocation) * MaxHandJointCount);
					FMemory::Memcpy(&RightHandAimState, &hand_joints[index].aim_state, sizeof(ps_common::HandAimState));
				}
			}
			break;
		case ps_common::SideType::kBoth:
			break;
		default: ;
		}
	}
}

void StreamingDataReceiveWrapper::NotifyBodyJointsUpdated(const ps_common::BodyJointLocations& body_joints)
{
	FMemory::Memcpy(&BodyLocations, &body_joints, sizeof(ps_common::BodyJointLocations));
}

void StreamingDataReceiveWrapper::NotifyBodyTrackingState(int32_t status_code, int32_t error_code)
{
}

float StreamingDataReceiveWrapper::GetJoystickX(EControllerHand DeviceHand) const
{
	return (DeviceHand == EControllerHand::Left) ? LeftJoystickX : RightJoystickX;
}

float StreamingDataReceiveWrapper::GetJoystickY(EControllerHand DeviceHand) const
{
	return (DeviceHand == EControllerHand::Left) ? LeftJoystickY : RightJoystickY;
}

float StreamingDataReceiveWrapper::GetTriggerValue(EControllerHand DeviceHand) const
{
	return (DeviceHand == EControllerHand::Left) ? LeftTriggerValue : RightTriggerValue;
}

float StreamingDataReceiveWrapper::GetGripValue(EControllerHand DeviceHand) const
{
	return (DeviceHand == EControllerHand::Left) ? LeftGripValue : RightGripValue;
}

float StreamingDataReceiveWrapper::GetStateValue(EControllerHand DeviceHand) const
{
	return (DeviceHand == EControllerHand::Left) ? LeftState : RightState;
}

bool StreamingDataReceiveWrapper::GetConnectionStatus(EControllerHand DeviceHand) const
{
	return (DeviceHand == EControllerHand::Left) ? bLeftControllerActive : bRightControllerActive;
}

bool StreamingDataReceiveWrapper::IsStreaming() const
{
	return bStreaming;
}

bool StreamingDataReceiveWrapper::GetStreamingConfig(StreamingConfig& streamingConfig) const
{
	if (PDCStreamingConfig.frame_rate >= 72
		&& PDCStreamingConfig.view_height > 0
		&& PDCStreamingConfig.view_width > 0)
	{
		streamingConfig = PDCStreamingConfig;
		return true;
	}
	return false;
}

bool StreamingDataReceiveWrapper::IsHandTrackingActived(EControllerHand DeviceHand) const
{
	return DeviceHand == EControllerHand::Left ? LeftHand_is_active : RightHand_is_active;
}

bool StreamingDataReceiveWrapper::IsEyeTrackingActived(EEyeSide Side) const
{
	return bEyeTrackingDataValid;
}

uint32 StreamingDataReceiveWrapper::GetHandJointCount(EControllerHand DeviceHand) const
{
	return DeviceHand == EControllerHand::Left ? LeftHand_joint_count : RightHand_joint_count;
}

TArray<ps_common::HandJointLocation> StreamingDataReceiveWrapper::GetHandJointInfos(EControllerHand DeviceHand) const
{
	return DeviceHand == EControllerHand::Left ? LeftHand_joint_infos : RightHand_joint_infos;
}

ps_common::HandAimState StreamingDataReceiveWrapper::GetHandAimState(EControllerHand DeviceHand) const
{
	return DeviceHand == EControllerHand::Left ? LeftHandAimState : RightHandAimState;
}

ps_common::EyeTrackingDataPICO StreamingDataReceiveWrapper::GetEyeTrackingDataPICO() const
{
	return EyeTrackingData;
}

ps_common::BodyJointLocations StreamingDataReceiveWrapper::GetBodyJointLocations() const
{
	return BodyLocations;
}

bool FPICOXRDPManager::bConnectToServiceSucceed = false;
StreamingDriverInterface::Ptr_t FPICOXRDPManager::StreamingDriverInterfacePtr = nullptr;
TSharedPtr<StreamingDataReceiveWrapper, ESPMode::ThreadSafe> FPICOXRDPManager::StreamingDataWrapperPtr = nullptr;
TArray<TRefCountPtr<ID3D11Texture2D>> FPICOXRDPManager::LeftDstSwapChainTextures;
TArray<TRefCountPtr<ID3D11Texture2D>> FPICOXRDPManager::RightDstSwapChainTextures;
TArray<void*> FPICOXRDPManager::LeftDstTextureHandles;
TArray<void*> FPICOXRDPManager::RightDstTextureHandles;
TArray<FTextureRHIRef> FPICOXRDPManager::LeftRHITextureSwapChain;
TArray<FTextureRHIRef> FPICOXRDPManager::RightRHITextureSwapChain;
FTextureRHIRef FPICOXRDPManager::LeftBindingTexture;
FTextureRHIRef FPICOXRDPManager::RightBindingTexture;
//Todo: Optimize opening method
bool FPICOXRDPManager::bHandTrackingStreamingEnabled = true;
bool FPICOXRDPManager::bEyeTrackingStreamingEnabled = true;
bool FPICOXRDPManager::bBodyTrackingStreamingEnabled = true;
TComPtr<IDXGIKeyedMutex> FPICOXRDPManager::LeftKeyedMutex;
TComPtr<IDXGIKeyedMutex> FPICOXRDPManager::RightKeyedMutex;

int32 FPICOXRDPManager::DefaultBodyJointSet = 0;
ps_common::BodyTrackingBoneLength FPICOXRDPManager::DefaultBoneLength;

StreamingConfig FPICOXRDPManager::streamingConfig;

bool FPICOXRDPManager::OnBeginPlayStartStreaming()
{
	StreamingConfig TempStreamingConfig;
	if (StreamingDataWrapperPtr->GetStreamingConfig(TempStreamingConfig))
	{
		streamingConfig = TempStreamingConfig;
	}

	if (StreamingDriverInterfacePtr
		&& StreamingDriverInterfacePtr->IsConnected()
		&& StreamingDriverInterfacePtr->StartStreaming(streamingConfig) != ResultCode::kOk)
	{
		PXR_LOGD(PxrUnreal, "PXR_LivePreview StartStreaming Failed!");
		return false;
	}

	PXR_LOGD(PxrUnreal, "PXR_LivePreview ConnectServer Successed!");

	return true;
}

bool FPICOXRDPManager::OnEndPlayStopStreaming()
{
	if (StreamingDriverInterfacePtr
		&& StreamingDriverInterfacePtr->IsConnected()
		&& StreamingDriverInterfacePtr->StopStreaming() != ResultCode::kOk)
	{
		PXR_LOGD(PxrUnreal, "PXR_LivePreview StopStreaming Failed!");
		return false;
	}

	return true;
}

bool FPICOXRDPManager::InitializeLivePreview()
{
	if (!CreateStreamingInterface())
	{
		return false;
	}

	if (!CreateSharedTexture2D())
	{
		PXR_LOGD(PxrUnreal, "PXR_LivePreview CreateSharedTexture2D Failed!");
		return false;
	}
	PXR_LOGD(PxrUnreal, "PXR_LivePreview CreateSharedTexture2D Successed!");

	streamingConfig.frame_rate = 72;
	streamingConfig.view_width = 1920;
	streamingConfig.view_height = 1920;

	return true;
}

void FPICOXRDPManager::ShutDownLivePreview()
{
	DisConnectStreamingServer();
}

bool FPICOXRDPManager::IsStreaming()
{
	return StreamingDriverInterfacePtr
		&& StreamingDriverInterfacePtr->IsConnected()
		&& StreamingDataWrapperPtr
		&& StreamingDataWrapperPtr->IsStreaming();
}

void FPICOXRDPManager::OnDeviceConnectionChanged(const DeviceInfo& device_info, int32 State)
{
	PXR_LOGD(PxrUnreal, "PXR_LivePreview OnDeviceConnectionChanged State:%d!", State);
	bConnectToServiceSucceed = State ? true : false;
}

bool FPICOXRDPManager::CreateStreamingInterface()
{
	if (!StreamingDataWrapperPtr.IsValid())
	{
		StreamingDataWrapperPtr = MakeShareable(new StreamingDataReceiveWrapper());
		StreamingDataWrapperPtr->OnDeviceConnectedResultsEvent.BindStatic(&OnDeviceConnectionChanged);
	}

	if (!StreamingDriverInterfacePtr && StreamingDataWrapperPtr.Get())
	{
		StreamingDriverInterfacePtr = StreamingDriverInterface::Create(StreamingDataWrapperPtr.Get());
	}

	return true;
}

bool FPICOXRDPManager::ConnectStreamingServer()
{
	if (!StreamingDriverInterfacePtr)
	{
		PXR_LOGD(PxrUnreal, "PXR_LivePreview ConnectStreamingServer Create Failed!");
		return false;
	}

	if (StreamingDriverInterfacePtr->ConnectToService() != ResultCode::kOk)
	{
		PXR_LOGD(PxrUnreal, "PXR_LivePreview ConnectStreamingServer ConnectToService Failed!");
		return false;
	}

	bConnectToServiceSucceed = true;

	return bConnectToServiceSucceed;
}

bool FPICOXRDPManager::DisConnectStreamingServer()
{
	if (!bConnectToServiceSucceed
		|| !StreamingDriverInterfacePtr
		|| StreamingDriverInterfacePtr->DisconnectFromService() != ResultCode::kOk)
	{
		return false;
	}
	return true;
}

bool FPICOXRDPManager::SetDisconnectFromServiceEvent(const FOnConnectFromServiceEvent& OnConnectFromServiceEvent)
{
	if (StreamingDataWrapperPtr)
	{
		StreamingDataWrapperPtr->OnConnectFromServiceEvent = OnConnectFromServiceEvent;
		return true;
	}
	return false;
}

bool FPICOXRDPManager::SetFovUpdatedFromServiceEvent(const FOnFovUpdatedFromServiceEvent& OnFovUpdatedFromServiceEvent)
{
	if (StreamingDataWrapperPtr)
	{
		StreamingDataWrapperPtr->OnFovUpdatedFromServiceEvent = OnFovUpdatedFromServiceEvent;
		return true;
	}
	return false;
}

bool FPICOXRDPManager::SetHandTrackingEnable(bool Enable)
{
	if (StreamingDriverInterfacePtr)
	{
		bHandTrackingStreamingEnabled = Enable;
		return Enable ? (StreamingDriverInterfacePtr->StartHandTracking() == ResultCode::kOk) : (StreamingDriverInterfacePtr->StopHandTracking() == ResultCode::kOk);
	}
	return true;
}

bool FPICOXRDPManager::SetEyeTrackingEnable(bool Enable)
{
	return false;
}

bool FPICOXRDPManager::SetBodyTrackingEnable(bool Enable)
{
	// if (StreamingDataWrapperPtr && IsStreaming())
	// {
	// 	bBodyTrackingStreamingEnabled = Enable;
	// 	return Enable ? (StreamingDriverInterfacePtr->StartBodyTracking(DefaultBodyJointSet, DefaultBoneLength) == ResultCode::kOk)
	// 		       : (StreamingDriverInterfacePtr->StopBodyTracking() == ResultCode::kOk);
	// }

	return false;
}

bool FPICOXRDPManager::SetSwiftMode(int32 Mode)
{
	DefaultBodyJointSet = Mode;
	if (StreamingDataWrapperPtr && IsStreaming())
	{
		//return StreamingDriverInterfacePtr->StartBodyTracking(Mode, DefaultBoneLength) == ResultCode::kOk;
	}

	return false;
}

// bool FPICOXRDPManager::SetBodyTrackingBoneLength(ps_common::BodyTrackingBoneLength& BoneLength)
// {
// 	return false;
// }
//
// bool FPICOXRDPManager::GetBodyTrackingPose(ps_common::BodyJointLocations& body_joints)
// {
// 	if (StreamingDataWrapperPtr && IsStreaming())
// 	{
// 		body_joints = StreamingDataWrapperPtr->GetBodyJointLocations();
// 	}
// 	return false;
// }

bool FPICOXRDPManager::StartBodyTrackingCalibApp(const char* calib_flag_string, int calib_mode)
{
	if (StreamingDriverInterfacePtr && IsStreaming())
	{
		return StreamingDriverInterfacePtr->StartBodyTrackingCalibApp(calib_flag_string, calib_mode) == ResultCode::kOk;
	}

	return false;
}

FTextureRHIRef FPICOXRDPManager::GetLeftBindingTexture()
{
	return LeftBindingTexture;
}

FTextureRHIRef FPICOXRDPManager::GetRightBindingTexture()
{
	return RightBindingTexture;
}

TArray<FTextureRHIRef> FPICOXRDPManager::GetLeftRHITextureSwapChain()
{
	return LeftRHITextureSwapChain;
}

TArray<FTextureRHIRef> FPICOXRDPManager::GetRightRHITextureSwapChain()
{
	return RightRHITextureSwapChain;
}

static const uint32 DPSwapChainLength = 1;
bool FPICOXRDPManager::SendMessage(int SwapChainIndex)
{
	TArray<ViewFrame> submit_ViewFrames;
	ViewFrame LeftViewFrame;
	ViewFrame RightViewFrame;
	LeftViewFrame.image.d3d11_shared_image.shared_handle = LeftDstTextureHandles[SwapChainIndex];
	RightViewFrame.image.d3d11_shared_image.shared_handle = RightDstTextureHandles[SwapChainIndex];

	LeftViewFrame.position = (StreamingDataWrapperPtr->GetHMDPosition());
	LeftViewFrame.orientation = (StreamingDataWrapperPtr->GetHMDRotation());

	RightViewFrame.position = (StreamingDataWrapperPtr->GetHMDPosition());
	RightViewFrame.orientation = (StreamingDataWrapperPtr->GetHMDRotation());

	submit_ViewFrames.Add(LeftViewFrame);
	submit_ViewFrames.Add(RightViewFrame);
	PXR_LOGV(PxrUnreal, "PXR_LivePreview Left Shared Handle ID:%lu", HandleToULong(LeftDstTextureHandles[SwapChainIndex]));
	PXR_LOGV(PxrUnreal, "PXR_LivePreview Right Shared Handle ID:%lu", HandleToULong(RightDstTextureHandles[SwapChainIndex]));
	if (StreamingDriverInterfacePtr->SubmitViewFrames(submit_ViewFrames.GetData(), 2) == ResultCode::kOk)
	{
		return true;
	}
	return false;
}

int ColorRGBA(int alpha, int red, int green, int blue)
{
	return (alpha << 24 | red << 16 | green << 8 | blue);
}

bool FPICOXRDPManager::CreateSharedTexture2D()
{
	UPICOXRDPSettings* HMDSettings = GetMutableDefault<UPICOXRDPSettings>();

	auto device = static_cast<ID3D11Device*>(GDynamicRHI->RHIGetNativeDevice());
	if (device == nullptr)
	{
		return false;
	}
	if (LeftBindingTexture && RightBindingTexture)
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC sharedTexture2Ddesc;
	ZeroMemory(&sharedTexture2Ddesc, sizeof(sharedTexture2Ddesc));
	sharedTexture2Ddesc.Height = 1920; //m_lastSize.Height;
	sharedTexture2Ddesc.Width = 1920; // m_lastSize.Width;
	sharedTexture2Ddesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	sharedTexture2Ddesc.ArraySize = 1;
	sharedTexture2Ddesc.MipLevels = 1;
	sharedTexture2Ddesc.Usage = D3D11_USAGE_DEFAULT;
	sharedTexture2Ddesc.SampleDesc.Count = 1;
	sharedTexture2Ddesc.CPUAccessFlags = 0;
	sharedTexture2Ddesc.BindFlags = D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;
	sharedTexture2Ddesc.MiscFlags = HMDSettings->GraphicQuality==EGraphicQuality::High?D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX:D3D11_RESOURCE_MISC_SHARED;

	uint32_t m_color = ColorRGBA(255, 108, 108, 255);

	std::vector<uint32_t> textureMap(1920 * 1920, m_color);
	uint32_t* pData = textureMap.data();

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = pData;
	sd.SysMemPitch = 1920 * sizeof(uint32_t);
	sd.SysMemSlicePitch = 1920 * 1920 * sizeof(uint32_t);

	for (uint32 SwapChainIter = 0; SwapChainIter < DPSwapChainLength; ++SwapChainIter)
	{
		TRefCountPtr<ID3D11Texture2D> LeftDstTexture;
		TRefCountPtr<ID3D11Texture2D> RightDstTexture;

		void* LeftDstTextureHandle;
		void* RightDstTextureHandle;

		TRefCountPtr<IDXGIResource> LeftDXGIResource;
		TRefCountPtr<IDXGIResource> RightDXGIResource;

		FTextureRHIRef LeftTempTexture;
		FTextureRHIRef RightTempTexture;

		if (device->CreateTexture2D(&sharedTexture2Ddesc, &sd, LeftDstTexture.GetInitReference()) != S_OK
			|| device->CreateTexture2D(&sharedTexture2Ddesc, &sd, RightDstTexture.GetInitReference()) != S_OK
		)
		{
			PXR_LOGD(PxrUnreal, "PXR_LivePreview DstSwapChainTextures:%d Created faild!", SwapChainIter);
			return false;
		}
		
		LeftDstTexture->QueryInterface(_uuidof(IDXGIKeyedMutex), (void**)&LeftKeyedMutex);
		RightDstTexture->QueryInterface(_uuidof(IDXGIKeyedMutex), (void**)&RightKeyedMutex);

		LeftDstSwapChainTextures.Add(LeftDstTexture);
		RightDstSwapChainTextures.Add(RightDstTexture);

		if (LeftDstSwapChainTextures[SwapChainIter] && RightDstSwapChainTextures[SwapChainIter])
		{
			LeftDstSwapChainTextures[SwapChainIter]->QueryInterface(LeftDXGIResource.GetInitReference());
			RightDstSwapChainTextures[SwapChainIter]->QueryInterface(RightDXGIResource.GetInitReference());

			const HRESULT LeftSharedHandleResult = LeftDXGIResource->GetSharedHandle(&LeftDstTextureHandle);
			const HRESULT RightSharedHandleResult = RightDXGIResource->GetSharedHandle(&RightDstTextureHandle);

			LeftDstTextureHandles.Add(LeftDstTextureHandle);
			RightDstTextureHandles.Add(RightDstTextureHandle);

			if (FAILED(LeftSharedHandleResult) || FAILED(RightSharedHandleResult))
			{
				PXR_LOGD(PxrUnreal, "PXR_LivePreview GetSharedHandle failed!");
				return false;
			}
			else
			{
				PXR_LOGD(PxrUnreal, "PXR_LivePreview Left Shared Handle ID:%lu", HandleToULong(LeftDstTextureHandle));
				PXR_LOGD(PxrUnreal, "PXR_LivePreview Right Shared Handle ID:%lu", HandleToULong(RightDstTextureHandle));

				CreateRHITexture(LeftDstSwapChainTextures[SwapChainIter], PF_R8G8B8A8, LeftTempTexture);
				CreateRHITexture(RightDstSwapChainTextures[SwapChainIter], PF_R8G8B8A8, RightTempTexture);
				LeftRHITextureSwapChain.Add((FTextureRHIRef&)LeftTempTexture);
				RightRHITextureSwapChain.Add((FTextureRHIRef&)RightTempTexture);
				if (LeftRHITextureSwapChain[SwapChainIter]->IsValid() || RightRHITextureSwapChain[SwapChainIter]->IsValid())
				{
					PXR_LOGD(PxrUnreal, "PXR_LivePreview SwapChainTextures OK");
				}
			}

			if (LeftBindingTexture == nullptr)
			{
				LeftBindingTexture = GDynamicRHI->RHICreateAliasedTexture((FTextureRHIRef&)LeftTempTexture);
			}
			if (RightBindingTexture == nullptr)
			{
				RightBindingTexture = GDynamicRHI->RHICreateAliasedTexture((FTextureRHIRef&)RightTempTexture);
			}
		}
		else
		{
			PXR_LOGD(PxrUnreal, "PXR_LivePreview Shared Texture Created faild!");
			return false;
		}
	}

	return true;
}

TComPtr<IDXGIKeyedMutex> FPICOXRDPManager::GetLeftKeyedMutex()
{
	return LeftKeyedMutex;
}

TComPtr<IDXGIKeyedMutex> FPICOXRDPManager::GetRightKeyedMutex()
{
	return RightKeyedMutex;
}


bool FPICOXRDPManager::LockKeyedMutex(UINT64 Key,DWORD TimeOut_ms)
{
	if((!LeftKeyedMutex||LeftKeyedMutex->AcquireSync(Key,TimeOut_ms)!= S_OK)
	||(!RightKeyedMutex||RightKeyedMutex->AcquireSync(Key,TimeOut_ms)!= S_OK))
	{
		PXR_LOGD(PxrUnreal, "PXR_LivePreview IDXGIKeyedMutex::AcquireSync Failed!");
		return false;
	}
	return true;
}

void FPICOXRDPManager::UnlockKeyedMutex(UINT64 Key)
{
	if (LeftKeyedMutex&&RightKeyedMutex)
	{
		LeftKeyedMutex->ReleaseSync(Key);
		RightKeyedMutex->ReleaseSync(Key);
	}
}


bool FPICOXRDPManager::CreateRHITexture(ID3D11Texture2D* OpenedSharedResource, EPixelFormat Format, FTextureRHIRef& OutTexture)
{
	FD3D11DynamicRHI* DynamicRHI = static_cast<FD3D11DynamicRHI*>(GDynamicRHI);
	OutTexture = DynamicRHI->RHICreateTexture2DFromResource(
		Format, TexCreate_RenderTargetable | TexCreate_ShaderResource | TexCreate_SRGB, FClearValueBinding::None, OpenedSharedResource).GetReference();
	return OutTexture.IsValid();
}

void FPICOXRDPManager::GetHMDPositionAndRotation(FVector& OutPostion, FQuat& OutQuat)
{
	if (StreamingDataWrapperPtr)
	{
		const ps_common::Vector position = StreamingDataWrapperPtr->GetHMDPosition();
		const ps_common::Quaternion rotation = StreamingDataWrapperPtr->GetHMDRotation();
		OutPostion.X = position.x();
		OutPostion.Y = position.y();
		OutPostion.Z = position.z();
		OutQuat.X = rotation.x();
		OutQuat.Y = rotation.y();
		OutQuat.Z = rotation.z();
		OutQuat.W = rotation.w();
	}
	else
	{
		OutPostion = FVector::ZeroVector;
		OutQuat = FQuat::Identity;
	}
}

void FPICOXRDPManager::GetControllerPositionAndRotation(EControllerHand DeviceHand, float WorldScale, FVector& OutPostion, FRotator& OutQuat)
{
	if (StreamingDataWrapperPtr)
	{
		const ps_common::Vector ControllerPosition = StreamingDataWrapperPtr->GetControllerPosition(DeviceHand);
		OutPostion.X = -ControllerPosition.z();
		OutPostion.Y = ControllerPosition.x();
		OutPostion.Z = ControllerPosition.y();
		OutPostion = OutPostion * WorldScale;
		const ps_common::Quaternion ControllerRotation = StreamingDataWrapperPtr->GetControllerRotation(DeviceHand);
		OutQuat = FQuat(ControllerRotation.z(), -ControllerRotation.x(), -ControllerRotation.y(), ControllerRotation.w()).Rotator();
	}
	else
	{
		OutPostion = FVector::ZeroVector;
		OutQuat = FRotator::ZeroRotator;
	}
}

bool FPICOXRDPManager::GetControllerConnectionStatus(EControllerHand DeviceHand)
{
	return StreamingDataWrapperPtr ? StreamingDataWrapperPtr->GetConnectionStatus(DeviceHand) : false;
}

int32 FPICOXRDPManager::GetControllerButtonStatus(EControllerHand DeviceHand)
{
	return StreamingDataWrapperPtr ? StreamingDataWrapperPtr->GetStateValue(DeviceHand) : 0;
}

void FPICOXRDPManager::GetControllerAxisValue(EControllerHand DeviceHand, double& JoyStickX, double& JoyStickY, float& TriggerValue, float& GripValue)
{
	if (StreamingDataWrapperPtr)
	{
		JoyStickX = (double)StreamingDataWrapperPtr->GetJoystickX(DeviceHand);
		JoyStickY = (double)StreamingDataWrapperPtr->GetJoystickY(DeviceHand);
		TriggerValue = StreamingDataWrapperPtr->GetTriggerValue(DeviceHand);
		GripValue = StreamingDataWrapperPtr->GetGripValue(DeviceHand);
	}
}

bool FPICOXRDPManager::IsConnectToServiceSucceed()
{
	if (bConnectToServiceSucceed && StreamingDriverInterfacePtr)
	{
		return StreamingDriverInterfacePtr->IsConnected();
	}
	return false;
}

bool FPICOXRDPManager::GetPICOXRFrustumDP(FPICOXRFrustumDP& PICOXRFrustumDP)
{
	ps_common::DeviceFovInfo TempDeviceFovInfo;
	if (StreamingDataWrapperPtr
		&& StreamingDataWrapperPtr->GetDeviceFovInfo(TempDeviceFovInfo))
	{
		PICOXRFrustumDP.FovUp = TempDeviceFovInfo.up();
		PICOXRFrustumDP.FovDown = TempDeviceFovInfo.down();
		PICOXRFrustumDP.FovLeft = TempDeviceFovInfo.left();
		PICOXRFrustumDP.FovRight = TempDeviceFovInfo.right();
		PICOXRFrustumDP.Type = static_cast<EEyeType>(TempDeviceFovInfo.side_type());

		PXR_LOGD(PxrUnreal, "PXR_LivePreview PICOXRFrustumDP.FovUp:%f", PICOXRFrustumDP.FovUp);
		PXR_LOGD(PxrUnreal, "PXR_LivePreview PICOXRFrustumDP.FovDown:%f", PICOXRFrustumDP.FovDown);
		PXR_LOGD(PxrUnreal, "PXR_LivePreview PICOXRFrustumDP.FovLeft:%f", PICOXRFrustumDP.FovLeft);
		PXR_LOGD(PxrUnreal, "PXR_LivePreview PICOXRFrustumDP.FovRight:%f", PICOXRFrustumDP.FovRight);
		PXR_LOGD(PxrUnreal, "PXR_LivePreview PICOXRFrustumDP.Type:%d", PICOXRFrustumDP.Type);

		return true;
	}

	return false;
}

bool FPICOXRDPManager::GetHandTrackerJointLocations(EControllerHand DeviceHand, ps_common::HandJointLocations& JointInfos)
{
	if (StreamingDataWrapperPtr && IsStreaming())
	{
		JointInfos.is_active = StreamingDataWrapperPtr->IsHandTrackingActived(DeviceHand);
		if (JointInfos.is_active)
		{
			JointInfos.joint_count = StreamingDataWrapperPtr->GetHandJointCount(DeviceHand);
			JointInfos.side_type = ((DeviceHand == EControllerHand::Left) ? ps_common::SideType::kLeft : ps_common::SideType::kRight);
			FMemory::Memcpy(JointInfos.joint_locations, StreamingDataWrapperPtr->GetHandJointInfos(DeviceHand).GetData(), sizeof(ps_common::HandJointLocation) * 32);
		}
		return true;
	}
	return false;
}

bool FPICOXRDPManager::GetHandTrackerAimState(EControllerHand DeviceHand, ps_common::HandAimState& AimState)
{
	if (StreamingDataWrapperPtr)
	{
		AimState = StreamingDataWrapperPtr->GetHandAimState(DeviceHand);
		return true;
	}
	return false;
}

bool FPICOXRDPManager::GetEyeTrackingData(ps_common::EyeTrackingDataPICO& EyeTrackingData)
{
	if (StreamingDataWrapperPtr)
	{
		EyeTrackingData = StreamingDataWrapperPtr->GetEyeTrackingDataPICO();
		return true;
	}
	return false;
}

bool FPICOXRDPManager::GetHandTrackerSettingState(bool& State)
{
	if (StreamingDataWrapperPtr)
	{
		State = bHandTrackingStreamingEnabled;
		return true;
	}
	return false;
}

bool FPICOXRDPManager::StartEyeTrackingSucceed(EEyeSide DeviceEye)
{
	if (StreamingDataWrapperPtr)
	{
		return StreamingDataWrapperPtr->IsEyeTrackingActived(DeviceEye);
	}
	
	return false;
}

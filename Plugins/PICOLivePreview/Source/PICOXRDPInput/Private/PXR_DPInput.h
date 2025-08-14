//Unreal® Engine, Copyright 1998 – 2023, Epic Games, Inc. All rights reserved.

#pragma once
#include "GenericPlatform/IInputInterface.h"
#include "XRMotionControllerBase.h"
#include "InputDevice.h"
#include "IHapticDevice.h"
#include "PICOXRDPHMD/Private/PXR_DPHMD.h"

#define ButtonEventNum 12

struct EPICOButton
{
	enum Type
	{
		RockerX,
		RockerY,
		Home,
		App,
		Rocker,
		VolumeUp,
		VolumeDown,
		Trigger,
		Power,
		AorX,
		BorY,
		Grip,
		RockerUp,
		RockerDown,
		RockerLeft,
		RockerRight,
		ButtonCount
	};
};

struct EPICOTouchButton
{
	enum Type
	{
		AorX,
		BorY,
		Rocker,
		Trigger,
		Thumbrest,
        ButtonCount
    };
};

struct EPICOXRControllerHandness
{
	enum Type
	{
		LeftController,
		RightController,
		ControllerCount
	};
};

enum  EPICOInputType:uint8
{
	Unknown = 0,
	G2      = 3,
	Neo2    = 4,
	Neo3    = 5,
};

enum  ETouchButtonStatus:uint16
{
	None = 0,
	XATouch = 2,
	YBTouch = 8,
	JoystickTouch = 32,
	TriggerTouch = 128,
	//GripTouch = 512,
	KBlankTouch = 8192
};

enum class EButtonStatus:uint16
{
	kNone = 0,
	kXAClick =1<< 0,
	kXATouch =1<< 1,
	kYBClick =1<< 2,
	kYBTouch =1<< 3,

	kJoystickClick =1<< 4,
	kJoystickTouch =1<< 5,
	kTriggerClick =1<< 6,
	kTriggerTouch =1<< 7,

	kHomeClick =1<< 8,
	kGripClick =1<< 9,
	kMenuButtonClick =1<< 12,

	kThumbrestTouch =1<< 13,
};

enum class EConnectionStatus
{
	kNotInitialized = 0,
	kDisconnected = 1,
	kConnected = 2,
	kConnecting = 3,
	kError = 4
};

//class FPICOXRHMD;
class FPICOXRDPInput :public IInputDevice,public FXRMotionControllerBase,public IHapticDevice,public TSharedFromThis<FPICOXRDPInput>
{
public:
	FPICOXRDPInput();
	virtual ~FPICOXRDPInput();

public: 
	// IInputDevice overrides
	virtual void Tick(float DeltaTime) override;
	virtual void SendControllerEvents() override;
	virtual void SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues &values) override;

	// IMotionController overrides
	virtual FName GetMotionControllerDeviceTypeName() const override;
	virtual bool GetControllerOrientationAndPosition(const int32 ControllerIndex, const FName MotionSource, FRotator& OutOrientation, FVector& OutPosition, float WorldToMetersScale) const override;
	virtual ETrackingStatus GetControllerTrackingStatus(const int32 ControllerIndex, const FName MotionSource) const override;

	// IHapticDevice overrides
	IHapticDevice* GetHapticDevice() override { return (IHapticDevice*)this; }
	virtual void SetHapticFeedbackValues(int32 ControllerId, int32 Hand, const FHapticFeedbackValues& Values) override;
	virtual void GetHapticFrequencyRange(float& MinFrequency, float& MaxFrequency) const override;
	virtual float GetHapticAmplitudeScale() const override;

	/*FPICOXRHMD* GetPICOXRHMD();*/
	int32 UPxr_GetControllerPower(int32 Handness);
	bool UPxr_GetControllerConnectState(int32 Handness);
	bool UPxr_GetControllerMainInputHandle(int32& Handness);
	bool UPxr_SetControllerMainInputHandle(int32 inHandness);

	void SetHeadPosition(FVector Position);
	void SetHeadOrientation(FQuat Orientation);
	
	static FVector OriginOffsetL;
	static FVector OriginOffsetR;

	static void RegisterKeys();
private:

	void SetKeyMapping();
	void ProcessButtonEvent();
	void ProcessButtonAxis();
	static void AddNonExistingKey(const TArray<FKey> &ExistAllKeys,const FKeyDetails& KeyDetails);

	FPICOXRHMDDP* PICOXRHMD=nullptr;
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;
	bool LeftConnectState;
	bool RightConnectState;
	FName Buttons[(int32)EPICOXRControllerHandness::ControllerCount][(int32)EPICOButton::ButtonCount];
	FName TouchButtons[(int32)EPICOXRControllerHandness::ControllerCount][(int32)EPICOTouchButton::ButtonCount];
	int32 LastLeftControllerButtonState[EPICOButton::ButtonCount] = { 0 };
	int32 LastRightControllerButtonState[EPICOButton::ButtonCount] = { 0 };
	int32 LastLeftTouchButtonState[EPICOTouchButton::ButtonCount] = { 0 };
	int32 LastRightTouchButtonState[EPICOTouchButton::ButtonCount] = { 0 };
	int32 LeftControllerPower;
	int32 RightControllerPower;
	bool bLeftButtonPressed = false;
	bool bRightButtonPressed = false;
	FVector2D LeftControllerTouchPoint;
	FVector2D RightControllerTouchPoint;
	float LeftControllerTriggerValue;
	float RightControllerTriggerValue;
	float LeftControllerGripValue;
	float RightControllerGripValue;
	uint32_t MainControllerHandle;
	FVector SourcePosition;
	FQuat SourceOrientation;
	EPICOInputType ControllerType;
};



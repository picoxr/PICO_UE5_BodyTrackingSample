#pragma once

#include <memory>

#include "ps_common/controller/controller_button.h"
#include "ps_common/controller/controller_state.h"
#include "ps_common/controller/controller_vibration.h"
#include "ps_common/device_fov_info.h"
#include "ps_common/device_pose.h"
#include "ps_common/device_tracking_origin.h"
#include "ps_common/driver_media_config.h"
#include "ps_common/graphics.h"
#include "ps_common/platform.h"
#include "ps_common/tracker/body_tracking.h"
#include "ps_common/tracker/eye_tracking.h"
#include "ps_common/tracker/hand_tracking.h"

#if defined(PS_DRIVER_SDK_LIB_SHARED) && \
    (defined(_WIN32) || defined(_WINDOWS) || defined(_WINDLL))
#ifdef PS_SDK_EXPORTS
#define PS_SDK_API __declspec(dllexport)
#else
#define PS_SDK_API __declspec(dllimport)
#endif  // PS_SDK_EXPORTS
#else
#define PS_SDK_API
#endif

namespace ps_driver_sdk {
enum class ResultCode {
  kUndefined = -1,
  kOk = 0,
  kInternalError = 10001,
  kInvalidParam = 10002,
  kCallError = 10003,
  kServiceError = 20001,
  kServiceDisconnect = 20002,
  kNetworkDisconnect = 20003
};
enum class StreamingState {
  kUndefinedStreamingState = 0,
  kStreamingLaunching,
  kStreamingReady,
  kStreamingFinished,
};
struct PS_SDK_API DeviceInfo {
  bool active = false;
  char device_id[256] = {0};
  char device_name[256] = {0};
  uint32_t display_physical_width = 0;
  uint32_t display_physical_height = 0;
  uint32_t lens_separation = 0;
};
enum class ViewConfigType {
  kUndefined = 0,
  kPrimaryMono = 1,
  kPrimaryStereo = 2
};
struct PS_SDK_API ViewFrame {
  uint64_t timestamp = 0;
  ps_common::GraphicsImage image;
  ps_common::Vector position;
  ps_common::Quaternion orientation;
};
struct PS_SDK_API StreamingConfig {
  ps_common::PixelFormat pixel_format =
      ps_common::PixelFormat::kR8G8B8A8_Uniform;
  ps_common::GraphicsBinding graphics_binding;
  ps_common::PlatformBinding platform_binding{};
  ViewConfigType view_config_type = ViewConfigType::kUndefined;
  int32_t view_width = 0;
  int32_t view_height = 0;
  int32_t frame_rate = 0;
};
class PS_SDK_API StreamingDriverInterfaceCallback {
 public:
  virtual void NotifyConnectToService(ResultCode result) = 0;
  virtual void NotifyDisconnectFromService(ResultCode reason) = 0;

  virtual void NotifyDeviceConnected(const DeviceInfo& device_info) = 0;
  virtual void NotifyDeviceDisconnected(const DeviceInfo& device_info) = 0;

  virtual void NotifyStreamingStateUpdated(StreamingState state) = 0;

  virtual void NotifyIPDUpdated(float ipd) = 0;
  virtual void NotifyFovUpdated(const ps_common::DeviceFovInfo& fov_info) = 0;
  virtual void NotifyDevicePoseUpdated(const ps_common::DevicePose pose_arr[],
                                       size_t arr_size) = 0;
  virtual void NotifyControllerButtonUpdated(
      const ps_common::ControllerButton& button) = 0;
  virtual void NotifyControllerStateUpdated(
      const ps_common::ControllerState controller_states[], size_t size) = 0;
  virtual void NotifyHandJointUpdated(
      const ps_common::HandJointLocations hand_joints[], size_t arr_size) = 0;
  virtual void NotifyEyeTrackingDataUpdated(
      const ps_common::EyeTrackingData& et_data) = 0;
  virtual void NotifyBodyJointsUpdated(
      const ps_common::BodyJointLocations& body_joints) = 0;
  virtual void NotifyBodyTrackingState(int32_t status_code,
                                       int32_t error_code) = 0;
  virtual ~StreamingDriverInterfaceCallback() = default;
};
class PS_SDK_API StreamingDriverInterface {
 public:
  typedef std::shared_ptr<StreamingDriverInterface> Ptr_t;
  static Ptr_t Create(StreamingDriverInterfaceCallback* callback);

  virtual ResultCode ConnectToService() = 0;
  virtual ResultCode DisconnectFromService() = 0;

  virtual ResultCode StartStreaming(const StreamingConfig& config) = 0;
  virtual ResultCode StopStreaming() = 0;

  virtual ResultCode SubmitViewFrames(const ViewFrame* frames,
                                      uint32_t frame_count) = 0;
  virtual ResultCode VibrateController(
      const ps_common::ControllerVibration& vibration) = 0;

  virtual ResultCode GetTrackingOrigin(
      ps_common::DeviceTrackingOrigin& origin) = 0;
  virtual ResultCode SetTrackingOrigin(
      ps_common::DeviceTrackingOrigin origin) = 0;

  virtual ResultCode GetFovInfo(ps_common::DeviceFovInfo& fov_info) = 0;
  virtual ResultCode GetDeviceInfo(DeviceInfo& device_info) = 0;
  virtual ResultCode GetControllerState(ps_common::SideType hand_side,
                                        ps_common::ControllerState& state) = 0;

  virtual ResultCode GetEyeTrackingSupported(bool& supported,
                                             int32_t& mode_count,
                                             int32_t* supported_modes) = 0;
  virtual ResultCode StartEyeTracking() = 0;
  virtual ResultCode StopEyeTracking() = 0;
  virtual ResultCode RequestEyeTrackingData() = 0;
  virtual ResultCode GetEyeTrackingState(bool& is_tracking,
                                         int32_t& state_code) = 0;

  virtual bool GetHandTrackingSupported() = 0;
  virtual ResultCode StartHandTracking() = 0;
  virtual ResultCode StopHandTracking() = 0;
  virtual ResultCode RequestHandTrackingData() = 0;

  virtual ResultCode StartBodyTracking(
      int32_t body_joint_set,
      const ps_common::BodyTrackingBoneLength& bone_length,
      int32_t alg_mode) = 0;
  virtual ResultCode StopBodyTracking() = 0;
  virtual ResultCode RequestBodyJoints() = 0;
  virtual ResultCode StartBodyTrackingCalibApp(const char* calib_flag_string,
                                               int calib_mode) = 0;
  virtual ResultCode RequestBodyTrackingState() = 0;

  virtual bool IsConnected() = 0;
  virtual StreamingState GetStreamingState() = 0;
  virtual ~StreamingDriverInterface() = default;
};
}  // namespace ps_driver_sdk
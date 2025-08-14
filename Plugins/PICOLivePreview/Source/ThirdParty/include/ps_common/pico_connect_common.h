#pragma once

#include <cstdint>

#include "ps_common/codec/video_codec_config.h"
#include "ps_common/device_type.h"
#include "ps_common/graphics.h"
#include "ps_common/platform.h"
#include "ps_common/ps_exports.h"
#include "ps_common/service_info.h"
#include "ps_common/stream/connection_config.h"
#include "ps_common/stream/video_stream_stats.h"
#include "ps_common/video/image_processing.h"
namespace ps_common {
enum class PICOConnectConnectState {
  kUndefined = -1,
  kPermitted = 0,
  kRefused = 1,
  kFinish = 2,
  kError = 3
};
const char* ToString(PICOConnectConnectState val);
enum class ListOperationType {
  kUndefined = -1,
  kAdd = 0,
  kRemove = 1,
  kUpdate = 2
};
enum class StreamingContentType {
  kUndefined = -1,
  kSteamVRApp = 0,
  kDesktop = 1,
};
enum class DisplayType {
  kUndefined = -1,
  kHardwareDisplay = 0,
  kVirtualDisplay = 1
};
struct StreamingContentInfo {
  StreamingContentType content_type = StreamingContentType::kUndefined;

  DisplayType display_type = DisplayType::kUndefined;
  char display_name[256] = {'\0'};
  int32_t display_width = 0;
  int32_t display_height = 0;
  int32_t refresh_rate_khz = 0;
  int32_t display_index = 0;

  char app_url[256] = {'\0'};
  char app_name[256] = {'\0'};
  char app_poster_url[1024 * 8] = {'\0'};
};
struct StreamingContentListItem {
  ListOperationType operation_type = ListOperationType::kUndefined;
  StreamingContentInfo content_info;
};
enum class StreamingContentStateType {
  kUndefined = -1,
  kSet = 0,
  kLaunched = 1,
  kRefused = 2,
  kFailed = 3
};
struct StreamingContentState {
  StreamingContentStateType state_type = StreamingContentStateType::kUndefined;
  StreamingContentInfo content_info;
  char description[1024]{0};
};

enum class TouchEventType {
  kUndefined = -1,
  kDown = 0,
  kUp = 1,
  kMove = 2,
  kHover = 3
};

struct TouchEvent {
  uint64_t timestamp = 0;
  TouchEventType event_type = TouchEventType::kUndefined;
  float pos_x = 0.f;
  float pos_y = 0.f;
};

enum class StreamingStateType {
  kUndefined = 0,
  kLaunching = 1,
  kRunning = 2,
  kPaused = 3,
  kFinished = 4
};
enum class AudioOutputType { kHMD = 0, kComputer = 1, kHMDAndComputer = 2 };
PS_COMMON_API const char* ToString(AudioOutputType val);
struct BodyTrackerSetting {
  int32_t joint_type = -1;
  bool enable_flag = false;
};
struct StreamingRealtimeConfig {
  ImageProcessingParam image_processing_param;
  bool auto_adapt_bitrate = false;
  bool enable_interpolated_frame = false;
  bool enable_supper_resolution = false;
  bool enable_body_tracking = false;
  uint32_t face_tracking_mode = 0;
  uint32_t face_tracking_transfer_protocol = 0;
  bool microphone_mute_flag = false;
  uint32_t microphone_volume = 0;
  AudioOutputType audio_output_type = AudioOutputType::kHMD;
  uint32_t controller_predicted_time_gain_ms = 0;
  uint32_t hmd_predicted_time_gain_ms = 0;
  bool enable_hand_tracking = false;
  bool enable_video_frame_buffering = false;
  ImageProcessingParam desktop_image_param;
  BodyTrackerSetting body_tracker_settings[14];
};
struct StreamingConfig {
  StreamingContentType streaming_content_type =
      StreamingContentType::kUndefined;
  GraphicsBinding graphics_binding;
  PlatformBinding platform_binding{};
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t frame_rate = 0;
  uint32_t bitrate_mbps = 0;
  VideoCodecType video_codec_type = VideoCodecType::kUndefined;
  StreamingRealtimeConfig default_realtime_config;
  bool enable_slice_mode = false;
  char additional_info_json[512] = {0};
};
struct StreamingState {
  StreamingStateType state_type = StreamingStateType::kUndefined;
  StreamingConfig streaming_config;
  char session_id[1024]{0};
};
struct StreamingStatistics {
  int64_t timestamp_ms = 0;
  // Note(by Alan Duan): Update by PC PICO Connect.
  bool auto_bitrate = false;
  bool enable_interpolated_frame = false;
  bool use_wire_network_flag = false;
  bool power_cable_plug_flag = false;
  int32_t gpu_total_usage = 0;
  int32_t gpu_encode_usage = 0;
  int32_t gpu_memory_usage = 0;

  // Note(by Alan Duan): Update by service.
  int32_t render_lag_ms = 0;
  int32_t signal_strength = 0;
  int32_t fov = 0;
  int32_t video_stream_count = 0;
  ps_common::VideoStreamStats video_stats[4];
  char gpu_device_type[1024]{0};
};
struct AccountLoginInfo {
  bool login_flag = false;
  char user_id[1024] = {'\0'};
  bool enable_usb_auto_connect = false;
  bool enable_network_auto_connect = false;
  ps_common::DeviceInfo external_device_info;
  bool private_access_flag = false;
};
struct SteamVRAppInfo {
  char app_key[1024] = {0};
  char app_name[1024] = {0};
};
struct PerformancePanelData {
  static constexpr size_t kMaxItemCount = 32;
  size_t item_count = 0;
  ps_base::DataBuffer item_names[kMaxItemCount];
  ps_base::DataBuffer item_values[kMaxItemCount];
};
struct PICOConnectAppInfo {
  int32_t app_version_code = 0;
  char app_version_name[64] = {0};
  char platform_type[64] = {0};
  char platform_version_name[512] = {0};
  char cpu_model[128] = {0};
  char memory_size[128] = {0};
  char graphics_card[128] = {0};
  char device_id[128] = {0};
};
struct DevicePoseAdjustment {
  DeviceType device_type = DeviceType::kUndefined;
  SideType side = SideType::kUndefined;
  double x = 0;
  double y = 0;
  double z = 0;
  double pitch = 0;
  double yaw = 0;
  double roll = 0;
};
}  // namespace ps_common

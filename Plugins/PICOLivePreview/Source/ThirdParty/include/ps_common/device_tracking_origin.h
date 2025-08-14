#pragma once
namespace ps_common {
enum class DeviceTrackingOrigin {
  kUndefined = -1,
  kEyeLevel = 0,
  kFloorLevel = 1,
  kStageLevel = 2
};
}
#pragma once
#include <stdint.h>

#include "device_type.h"
#include "ps_base/transport/data_buffer.h"
#include "ps_common/ps_exports.h"

namespace ps_common {
class PS_COMMON_API DeviceBattery {
 public:
  DeviceBattery() = default;
  DeviceBattery(const DeviceBattery& other) = default;
  ~DeviceBattery() = default;
  DeviceBattery& operator=(const DeviceBattery& other) = default;
  bool operator==(const DeviceBattery& other);
  bool operator!=(const DeviceBattery& other);
  void Reset();
  ps_base::DataBuffer ToString() const;

  uint8_t GetBatteryPercent_() const;
  void SetBatteryPercent_(uint8_t val);

  DeviceType device_type() const;
  DeviceType& mutable_device_type();

  SideType side_type() const;
  SideType& mutable_side_type();

  uint8_t battery_percent() const;
  uint8_t& mutable_battery_percent();

  bool active() const;
  bool& mutable_active();

 private:
  bool active_ = false;
  uint8_t battery_percent_ = 0;
  DeviceType device_type_ = DeviceType::kUndefined;
  SideType side_type_ = SideType::kUndefined;
};
}  // namespace ps_common
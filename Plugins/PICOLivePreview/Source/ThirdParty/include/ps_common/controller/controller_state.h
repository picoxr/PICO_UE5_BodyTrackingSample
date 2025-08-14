#pragma once
#include <cstdint>

#include "ps_base/transport/data_buffer.h"
#include "ps_common/device_type.h"
#include "ps_common/ps_exports.h"
namespace ps_common {
class PS_COMMON_API ControllerState {
 public:
  ControllerState() = default;
  ControllerState(const ControllerState& other) = default;
  ~ControllerState() = default;
  ControllerState& operator=(const ControllerState& other) = default;
  void Reset();
  [[nodiscard]] ps_base::DataBuffer ToString() const;

  [[nodiscard]] SideType side_type() const;
  SideType& mutable_side_type();

  [[nodiscard]] bool active() const;
  bool& mutable_active();

  [[nodiscard]] uint8_t battery_percent() const;
  uint8_t& mutable_battery_percent();

 private:
  SideType side_type_ = SideType::kUndefined;
  bool active_ = false;
  uint8_t battery_percent_ = 0;
};
}  // namespace ps_common
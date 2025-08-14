#pragma once
#include <stdint.h>

#include "ps_common/ps_exports.h"

namespace ps_common {
class PS_COMMON_API HmdState {
 public:
  HmdState() = default;
  ~HmdState() = default;
  HmdState(const HmdState& other);

  uint8_t GetBatteryPercent_() const;
  void SetBatteryPercent_(uint8_t val);

  void Reset();

  uint8_t battery_percent() const;
  uint8_t& battery_percent();

  HmdState& operator=(const HmdState& other);

 private:
  uint8_t battery_percent_ = 0;
};
}  // namespace ps_common
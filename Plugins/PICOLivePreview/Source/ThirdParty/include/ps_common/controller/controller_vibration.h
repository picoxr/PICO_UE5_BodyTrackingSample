#pragma once
#include <cstdint>

#include "ps_base/transport/data_buffer.h"
#include "ps_common/device_type.h"
#include "ps_common/ps_exports.h"
namespace ps_common {
class PS_COMMON_API ControllerVibration {
 public:
  ControllerVibration() = default;
  ControllerVibration(const ControllerVibration& other) = default;
  ~ControllerVibration() = default;
  ControllerVibration& operator=(const ControllerVibration& other) = default;
  bool operator==(const ControllerVibration& other) const;
  bool operator!=(const ControllerVibration& other) const;
  void Reset();
  [[nodiscard]] ps_base::DataBuffer ToString() const;

  [[nodiscard]] SideType side_type() const;
  SideType& mutable_side_type();

  [[nodiscard]] float amplitude() const;
  float& mutable_amplitude();

  [[nodiscard]] float duration() const;
  float& mutable_duration();

  [[nodiscard]] float frequency() const;
  float& mutable_frequency();

 private:
  SideType side_type_ = SideType::kUndefined;
  float amplitude_ = 0.;
  float duration_ = 0.;
  float frequency_ = 0.;
};
}  // namespace ps_common
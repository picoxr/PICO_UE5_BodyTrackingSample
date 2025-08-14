#pragma once

#include "ps_base/transport/data_buffer.h"
#include "ps_common/device_type.h"
#include "ps_common/motion_param.h"
#include "ps_common/ps_exports.h"
#include "ps_common/quaternion.h"

namespace ps_common {
class PS_COMMON_API DevicePose {
 public:
  DevicePose() = default;
  DevicePose(const DevicePose& other) = default;
  ~DevicePose() = default;
  DevicePose& operator=(const DevicePose& other) = default;
  void Reset();
  [[nodiscard]] ps_base::DataBuffer ToString() const;

  [[nodiscard]] DeviceType device_type() const;
  DeviceType& mutable_device_type();

  [[nodiscard]] SideType side_type() const;
  SideType& mutable_side_type();

  [[nodiscard]] bool active() const;
  bool& mutable_active();

  [[nodiscard]] int64_t timestamp_ns() const;
  int64_t& mutable_timestamp_ns();

  [[nodiscard]] const Vector& position() const;
  Vector& mutable_position();

  [[nodiscard]] const Quaternion& rotation() const;
  Quaternion& mutable_rotation();

  [[nodiscard]] const MotionParam& linear() const;
  MotionParam& mutable_linear();

  [[nodiscard]] const MotionParam& angular() const;
  MotionParam& mutable_angular();

 private:
  DeviceType device_type_ = DeviceType::kUndefined;
  SideType side_ = SideType::kUndefined;
  bool active_ = false;
  int64_t timestamp_ns_ = 0;
  Vector position_;
  Quaternion rotation_;
  MotionParam linear_;
  MotionParam angular_;
};
}  // namespace ps_common

#pragma once
#include "ps_base/transport/data_buffer.h"
#include "ps_common/device_type.h"
#include "ps_common/ps_exports.h"
namespace ps_common {
class PS_COMMON_API DeviceFovInfo {
 public:
  DeviceFovInfo() = default;
  DeviceFovInfo(const DeviceFovInfo& other) = default;
  ~DeviceFovInfo() = default;
  DeviceFovInfo& operator=(const DeviceFovInfo& other) = default;
  bool operator==(const DeviceFovInfo& other);
  bool operator!=(const DeviceFovInfo& other);
  void Reset();
  [[nodiscard]] ps_base::DataBuffer ToString() const;

  [[nodiscard]] SideType side_type() const;
  SideType& mutable_side_type();

  [[nodiscard]] float left() const;
  float& mutable_left();

  [[nodiscard]] float right() const;
  float& mutable_right();

  [[nodiscard]] float up() const;
  float& mutable_up();

  [[nodiscard]] float down() const;
  float& mutable_down();

 private:
  SideType side_type_ = SideType::kUndefined;
  float left_ = 0.;
  float right_ = 0.;
  float up_ = 0.;
  float down_ = 0.;
};
}  // namespace ps_common
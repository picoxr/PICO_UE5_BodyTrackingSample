#pragma once
#include <cstdint>

#include "ps_base/transport/data_buffer.h"
#include "ps_common/device_type.h"
#include "ps_common/ps_exports.h"
namespace ps_common {
class PS_COMMON_API ControllerButton {
 public:
  enum class KeyStateBitmask {
    kAXClick = 0x0001,
    kAXTouch = 0x0002,
    kBYClick = 0x0004,
    kBYTouch = 0x0008,
    kRockerTouch = 0x0010,
    kRockerClick = 0x0020,
    kTriggerClick = 0x0040,
    kTriggerTouch = 0x0080,
    kHomeClick = 0x0100,
    kGripClick = 0x0200,
    kReserved1 = 0x0400,
    kReserved2 = 0x0800,
    kMenuClick = 0x1000,
    kThumbrestTouch = 0x2000
  };

  ControllerButton() = default;
  ~ControllerButton() = default;
  ControllerButton(const ControllerButton& other) = default;
  ControllerButton& operator=(const ControllerButton& other) = default;
  bool operator==(const ControllerButton& other) const;
  bool operator!=(const ControllerButton& other) const;
  void Reset();
  [[nodiscard]] ps_base::DataBuffer ToString() const;

  [[nodiscard]] int64_t timestamp_ms() const;
  int64_t& mutable_timestamp_ms();

  [[nodiscard]] bool active() const;
  bool& mutable_active();

  [[nodiscard]] SideType side_type() const;
  SideType& mutable_side_type();

  /**
   * @return bitmask see @enum class KeyStateBitmask
   */
  [[nodiscard]] uint32_t state() const;
  uint32_t& mutable_state();

  /**
   * @return float -1~1
   */
  [[nodiscard]] float joystick_x() const;
  float& mutable_joystick_x();
  /**
   * @return float -1~1
   */
  [[nodiscard]] float joystick_y() const;
  float& mutable_joystick_y();

  /**
   * @return float 0~1
   */
  [[nodiscard]] float trigger_value() const;
  float& mutable_trigger_value();

  /**
   * @return float 0~1
   */
  [[nodiscard]] float grip_value() const;
  float& mutable_grip_value();

  /**
   * no button has touched or pressed
   * @return
   */
  [[nodiscard]] bool idle() const;

 private:
  int64_t timestamp_ms_ = 0;
  SideType side_type_ = SideType::kUndefined;
  bool active_ = false;
  uint32_t state_ = 0;
  float joystick_x_ = 0;
  float joystick_y_ = 0;
  float trigger_value_ = 0;
  float grip_value_ = 0;
};
}  // namespace ps_common
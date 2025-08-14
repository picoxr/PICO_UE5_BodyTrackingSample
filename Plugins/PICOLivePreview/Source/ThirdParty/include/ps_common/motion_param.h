#pragma once
#include "ps_common/vector.h"

namespace ps_common {
class PS_COMMON_API MotionParam {
 public:
  MotionParam() = default;
  ~MotionParam() = default;
  MotionParam(const MotionParam& other) = default;
  MotionParam& operator=(const MotionParam& other) = default;
  bool operator==(const MotionParam& other);
  bool operator!=(const MotionParam& other);
  void Reset();
  [[nodiscard]] ps_base::DataBuffer ToString() const;

  [[nodiscard]] const Vector& velocity() const;
  Vector& mutable_velocity();

  [[nodiscard]] const Vector& acceleration() const;
  Vector& mutable_acceleration();

 private:
  Vector velocity_;
  Vector acceleration_;
};
}  // namespace ps_common
#pragma once
//
// Created by Alan Duan on 2023/10/5.
//
#include "ps_common/ps_exports.h"
#include "ps_common/quaternion.h"
#include "ps_common/vector.h"
namespace ps_common {
class PS_COMMON_API Pose {
 public:
  Pose() = default;
  ~Pose() = default;
  Pose(const Quaternion& orientation, const Vector& position);
  Pose(const Pose& other) = default;

  Pose& operator=(const Pose& other) = default;
  bool operator==(const Pose& other) const;
  bool operator!=(const Pose& other) const;
  void Reset();
  [[nodiscard]] ps_base::DataBuffer ToString() const;

  [[nodiscard]] const Quaternion& orientation() const;
  Quaternion& mutable_orientation();

  [[nodiscard]] const Vector& position() const;
  Vector& mutable_position();
 private:
  Quaternion orientation_;
  Vector position_;
};
}  // namespace ps_common

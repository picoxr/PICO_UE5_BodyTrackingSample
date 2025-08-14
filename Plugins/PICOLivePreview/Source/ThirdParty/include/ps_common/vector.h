#pragma once
#include "ps_base/transport/data_buffer.h"
#include "ps_common/ps_exports.h"

namespace ps_common {
class PS_COMMON_API Vector {
 public:
  Vector() = default;
  ~Vector() = default;
  Vector(float x, float y, float z);
  Vector(const Vector& other) = default;

  Vector& operator=(const Vector& other) = default;
  bool operator==(const Vector& other) const;
  bool operator!=(const Vector& other) const;
  void Set(float x, float y, float z);
  void Reset();
  [[nodiscard]] ps_base::DataBuffer ToString() const;
  [[nodiscard]] float x() const;
  float& mutable_x();

  [[nodiscard]] float y() const;
  float& mutable_y();

  [[nodiscard]] float z() const;
  float& mutable_z();

 private:
  float x_ = 0.;
  float y_ = 0.;
  float z_ = 0.;
};
}  // namespace ps_common
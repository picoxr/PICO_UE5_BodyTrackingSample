#pragma once
#include "ps_base/transport/data_buffer.h"
#include "ps_common/ps_exports.h"
namespace ps_common {
class PS_COMMON_API Quaternion {
 public:
  Quaternion() = default;
  ~Quaternion() = default;
  Quaternion(const Quaternion& other) = default;
  Quaternion(float w, float x, float y, float z);

  Quaternion& operator=(const Quaternion& other) = default;
  bool operator==(const Quaternion& other) const;
  bool operator!=(const Quaternion& other) const;
  void Set(float x, float y, float z, float w);
  void Reset();
  [[nodiscard]] ps_base::DataBuffer ToString() const;

  [[nodiscard]] float x() const;
  float& mutable_x();

  [[nodiscard]] float y() const;
  float& mutable_y();

  [[nodiscard]] float z() const;
  float& mutable_z();

  [[nodiscard]] float w() const;
  float& mutable_w();

 private:
  float w_ = 0.;
  float x_ = 0.;
  float y_ = 0.;
  float z_ = 0.;
};
}  // namespace ps_common
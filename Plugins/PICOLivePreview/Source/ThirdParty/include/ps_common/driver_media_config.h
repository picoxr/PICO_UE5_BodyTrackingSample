#pragma once
#include <cstdint>

#include "ps_base/transport/data_buffer.h"
#include "ps_common/ps_exports.h"
namespace ps_common {
class PS_COMMON_API DriverMediaConfig {
 public:
  DriverMediaConfig() = default;
  ~DriverMediaConfig() = default;
  DriverMediaConfig(const DriverMediaConfig& other) = default;
  DriverMediaConfig& operator=(const DriverMediaConfig& other) = default;
  bool operator==(const DriverMediaConfig& other) const;
  bool operator!=(const DriverMediaConfig& other) const;

  void Set_(int32_t width, int32_t height, int32_t frame_rate);
  void Reset();
  [[nodiscard]] ps_base::DataBuffer ToString() const;

  [[nodiscard]] int32_t render_width() const;
  int32_t& mutable_render_width();

  [[nodiscard]] int32_t render_height() const;
  int32_t& mutable_render_height();

  [[nodiscard]] int32_t frame_rate() const;
  int32_t& mutable_frame_rate();

 private:
  int32_t render_width_ = 0;
  int32_t render_height_ = 0;
  int32_t frame_rate_ = 0;
};
}  // namespace ps_common
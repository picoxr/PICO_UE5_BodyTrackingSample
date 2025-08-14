#pragma once

#include <cstdint>

#include "ps_base/transport/data_buffer.h"
namespace ps_common {
struct RtpParam {
  uint8_t payload_type = 255;
  uint32_t ssrc = 0;
};

enum class PackageType {
  kUndefined = 0,
  kRtp,
  kRaw,
  kDynamic,
};

struct PackageConfig {
  PackageType package_type = PackageType::kUndefined;
  RtpParam rtp_param;
  [[nodiscard]] ps_base::DataBuffer ToString() const;
  bool operator==(const PackageConfig& other) const;
};
}  // namespace ps_common
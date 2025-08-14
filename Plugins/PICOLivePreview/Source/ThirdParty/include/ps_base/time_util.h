#pragma once
#include <cstdint>

#include "ps_base/ps_exports.h"

namespace ps_base {
class PS_BASE_API TimeUtil {
 public:
  static int64_t TimestampNs();
  static int64_t TimestampUs();
  static int64_t TimestampMs();
};
}  // namespace ps_base
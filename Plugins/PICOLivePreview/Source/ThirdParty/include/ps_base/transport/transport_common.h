#pragma once

#include <stdint.h>

#include "ps_base/ps_exports.h"

namespace ps_base {
enum class TransportType {
  kUndefined = -1,
  kTcp = 0,
  kUdp = 1,
  kAoa = 2,
  kFileDescriptor = 3,
  kUsb = 4,
  kDomain = 5,
  kBinder = 6,
};

typedef uint8_t TransportChannelId;
static const TransportChannelId kInvalidChannelId = 0xFF;

PS_BASE_API const char* ToString(TransportType type);
}  // namespace ps_base
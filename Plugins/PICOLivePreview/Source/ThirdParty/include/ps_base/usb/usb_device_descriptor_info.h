#pragma once
#include <stdint.h>

#include "ps_base/ps_exports.h"

namespace ps_base {
struct PS_BASE_API UsbDeviceDescriptorInfo {
  uint16_t vendor_id = 0;
  uint16_t product_id = 0;
  uint8_t interface_num = 0;
  uint8_t interface_class = 0;
  uint8_t interface_sub_class = 0;
  uint8_t interface_protocol = 0;
};
}  // namespace ps_base
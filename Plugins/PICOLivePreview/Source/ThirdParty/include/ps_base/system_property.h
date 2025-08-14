#pragma once

#include <string>

#include "ps_base/ps_exports.h"

#if defined(__ANDROID__)
#include <sys/system_properties.h>

#endif
namespace ps_base {
class PS_BASE_API SystemProperty {
 public:
#if defined(__ANDROID__)
#define ENABLE_DEBUG_DECODER_MASK 0x01
#define ENABLE_DEBUG_VIDEO_POST_PROCESS_MASK 0x02
#define ENABLE_DEBUG_XR_MODULE_MASK 0x04

  static std::string GetAndroidSerialNumber() {
    char serial_number[128] = {};
    bool res = __system_property_get("ro.serialno", serial_number) != 0;
    if (!res) {
      return {};
    }

    return {serial_number};
  };

  static int32_t GetAndroidVersion() {
    char version[128] = {};
    bool res = __system_property_get("ro.build.version.release", version) != 0;
    if (!res) {
      return -1;
    }
    return std::stoi({version});
  };

  static int32_t GetIntSystemProperty(const char* property_name) {
    if (!property_name) {
      PS_WARN("property name can not null")
      return -1;
    }
    char property_value[128] = {};
    bool res = __system_property_get(property_name, property_value) != 0;
    if (!res) {
      return -1;
    }
    int result;
    try {
      result = std::stoi({property_value});
    } catch (const std::invalid_argument& e) {
      return -1;
    }
    return result;
  };

  /**
   * get module debug mode by system property
   * @param mask
   * @return 1
   */
  static bool DebugModeEnabled(uint64_t mask) {
    static constexpr char kPropertyDecoderDebugEnable[128] =
        "pxr.streaming.debug.enable";
    char mask_value[128] = {};
    bool res =
        __system_property_get(kPropertyDecoderDebugEnable, mask_value) != 0;
    if (!res) {
      return false;
    }
    try {
      res = (std::stoi(mask_value) & mask) > 0;
    } catch (const std::invalid_argument& e) {
      return false;
    }
    return res;
  };

#endif
};
}  // namespace ps_base
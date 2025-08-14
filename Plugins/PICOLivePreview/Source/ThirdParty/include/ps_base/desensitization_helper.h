#pragma once

#include <string>
#include "ps_base/ps_exports.h"

namespace ps_base {
class PS_BASE_API DesensitizationHelper {
 public:
  static std::string Desensitize(const std::string& src);
};
}  // namespace ps_base

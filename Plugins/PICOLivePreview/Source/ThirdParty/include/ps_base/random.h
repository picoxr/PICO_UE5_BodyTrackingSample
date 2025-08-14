#pragma once

#include <string>

#include "ps_base/ps_exports.h"

namespace ps_base {
class PS_BASE_API Random {
 public:
  //
  // brief: Generate random string.
  // param: [in] length     - Random string length, less than 256.
  //       [in] containNum - If true, random string will contain number,
  //                         Otherwise only contain letters.
  static std::string RandomString(size_t length, bool hasNum = true);
  static int64_t RandomNumber(int64_t min, int64_t max);
};
}  // namespace ps_base
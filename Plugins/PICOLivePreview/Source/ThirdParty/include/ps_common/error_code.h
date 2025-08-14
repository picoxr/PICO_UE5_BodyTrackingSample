//
// Created by Alan Duan on 2024/6/17.
//
#pragma once
#include <cstdint>
#include <string>

#include "ps_common/ps_exports.h"
namespace ps_common {
typedef int32_t ErrorCode;
// Module Define
// 0x01 PS Engine
//
// 0x10 External Service
// 0x11 HMD Service
//
// 0x20 PICO Client
// 0x21 PICO Connect External SDK
// 0x22 PICO Connect HMD SDK
// 0x23 PICO Driver SDK
//
// 0x30 PICO Control SDK
// 0x31 PICO Driver SDK
class PS_COMMON_API Result {
 public:
  // Note(by Alan Duan): Max length of description is 128.
  static constexpr int32_t kMaxDescriptionLen = 128;
  explicit Result(uint8_t module, ErrorCode code, const char* description);
  [[nodiscard]] ErrorCode error_code() const;
  [[nodiscard]] const char* description() const;
  explicit operator const char*() const;
  explicit operator const int32_t() const;
  explicit operator bool() const;
  Result(const Result& other) = default;
  Result& operator=(const Result& other) = default;
  bool operator==(const Result& other) const;
  bool operator!=(const Result& other);

 private:
  ErrorCode code_ = 0;
  std::string description_;
};
const Result kOk(0, 0, "Success");
}  // namespace ps_common

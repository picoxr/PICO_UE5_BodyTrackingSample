#pragma once
//
// Created by Alan Duan on 2023/10/19.
//
#include "ps_base/transport/data_buffer.h"
#include "ps_common/ps_exports.h"
namespace ps_common {
enum class ColorModeType
{
  kDefault = 0,
  kREC2020 = 1,
  kHSV1_4 = 2
};
PS_COMMON_API ps_base::DataBuffer ToString(ColorModeType val);
struct PS_COMMON_API ImageProcessingParam {
  float bright = 1.0f;
  float saturation = 1.0f;
  float contrast = 1.0f;
  float gamma = 1.0f;
  float sharpening = 0.0f;
  ColorModeType color_mode = ColorModeType::kDefault;
  [[nodiscard]] ps_base::DataBuffer ToString() const;
};

}  // namespace ps_common

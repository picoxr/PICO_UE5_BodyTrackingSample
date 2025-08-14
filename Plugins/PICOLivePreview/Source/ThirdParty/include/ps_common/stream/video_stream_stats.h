#pragma once
//
// Created by Alan Duan on 2023/10/18.
//
#include <cstdint>

#include "ps_base/transport/data_buffer.h"
#include "ps_common/codec/video_codec_config.h"
#include "ps_common/ps_exports.h"
namespace ps_common {
struct PS_COMMON_API VideoStreamStats {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t bitrate_kbps = 0;
  uint32_t frame_rate = 0;
  VideoCodecType codec_type = VideoCodecType::kUndefined;
  int64_t encode_time_cost_ms = 0;
  int64_t decode_time_cost_ms = 0;
  int64_t capture_time_cost_ms = 0;
  int64_t total_time_cost_ms = 0;
  [[nodiscard]] ps_base::DataBuffer ToString() const;
};
}  // namespace ps_common

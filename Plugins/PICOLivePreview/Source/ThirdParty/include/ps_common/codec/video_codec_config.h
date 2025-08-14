#pragma once
#include <cstdint>

#include "ps_base/transport/data_buffer.h"
#include "ps_common/ps_exports.h"

namespace ps_common {

enum class VideoCodecType {
  kUndefined = 0,
  kH264,
  kH265,
  kAV1,
};

enum class VideoCodecMode {
  kUndefined = 0,
  kSync,
  kAsync,
  kHybrid  // Only for decoding. Input sync & Output async. slice-decoding only
           // in this mode.
};

// bitrate control mode
enum class VideoCodecBitRateMode {
  BITRATE_MODE_CQ = 0,  // Constant quality mode
  BITRATE_MODE_VBR,     // Variable bitrate mode
  BITRATE_MODE_CBR,     // Constant bitrate mode
};
class PS_COMMON_API VideoCodecConfig {
 public:
  VideoCodecConfig() = default;
  ~VideoCodecConfig() = default;
  VideoCodecConfig(const VideoCodecConfig& other);

  [[nodiscard]] VideoCodecType GetCodecType_() const;
  void SetCodecType_(VideoCodecType type);

  [[nodiscard]] VideoCodecMode GetCodecMode_() const;
  void SetCodecMode_(VideoCodecMode mode);

  [[nodiscard]] int32_t GetWidth_() const;
  void SetWidth_(int32_t width);

  [[nodiscard]] int32_t GetHeight_() const;
  void SetHeight_(int32_t height);

  [[nodiscard]] int32_t GetBitrate_() const;
  void SetBitrate_(int32_t bitrate);

  [[nodiscard]] int32_t GetFps_() const;
  void SetFps_(int32_t fps);

  [[nodiscard]] int32_t GetColorFormat() const;
  void SetColorFormat(int32_t color_format);

  [[nodiscard]] VideoCodecBitRateMode GetBitRateMode_() const;
  void SetBitRateMode_(VideoCodecBitRateMode mode);

  [[nodiscard]] bool GetSliceEnabled_() const;
  void SetSliceEnabled_(bool enable);

  [[nodiscard]] VideoCodecType codec_type() const;
  VideoCodecType& codec_type();

  [[nodiscard]] VideoCodecMode codec_mode() const;
  VideoCodecMode& codec_mode();

  [[nodiscard]] int32_t width() const;
  int32_t& width();

  [[nodiscard]] int32_t height() const;
  int32_t& height();

  [[nodiscard]] int32_t bitrate_kbps() const;
  int32_t& bitrate_kbps();

  [[nodiscard]] int32_t fps() const;
  int32_t& fps();

  [[nodiscard]] int32_t color_format() const;
  int32_t& color_format();

  [[nodiscard]] VideoCodecBitRateMode bitrate_mode() const;
  VideoCodecBitRateMode& bitrate_mode();

  [[nodiscard]] bool slice_enabled() const;
  bool& slice_enabled();

  VideoCodecConfig& operator=(const VideoCodecConfig& other);
  bool operator==(const VideoCodecConfig& other) const;
  [[nodiscard]] ps_base::DataBuffer ToString() const;

 private:
  VideoCodecType codec_type_ = VideoCodecType::kUndefined;
  VideoCodecMode codec_mode_ = VideoCodecMode::kUndefined;
  int32_t width_ = 0;
  int32_t height_ = 0;
  int32_t bitrate_kbps_ = 0;
  int32_t fps_ = 0;
  int32_t color_format_ = 0;
  VideoCodecBitRateMode bitrate_mode_ = VideoCodecBitRateMode::BITRATE_MODE_CBR;
  bool slice_enabled_ = false;
};
}  // namespace ps_common

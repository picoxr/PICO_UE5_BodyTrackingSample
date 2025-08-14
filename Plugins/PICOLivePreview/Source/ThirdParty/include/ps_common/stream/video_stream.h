#pragma once
#include "ps_common/codec/video_codec_config.h"
#include "ps_common/graphics.h"
#include "ps_common/ps_exports.h"
#include "ps_common/stream/connection_config.h"
#include "ps_common/stream/package_config.h"

namespace ps_common {
class PS_COMMON_API VideoStream {
 public:
  VideoStream() = default;
  VideoStream(const VideoStream& other);

  VideoStream& operator=(const VideoStream& other);
  bool operator==(const VideoStream& other);
  [[nodiscard]] const ps_base::DataBuffer& GetStreamId_() const;
  void SetStreamId_(const ps_base::DataBuffer& id);

  [[nodiscard]] const ps_base::DataBuffer& GetDescription_() const;
  void SetDescription_(const ps_base::DataBuffer& val);

  [[nodiscard]] VideoCodecConfig GetVideoCodecConfig_() const;
  void SetVideoCodecConfig_(const VideoCodecConfig& codec_config);

  [[nodiscard]] ConnectionConfig GetConnectionConfig_() const;
  void SetConnectionConfig_(const ConnectionConfig& conn_config);
  [[nodiscard]] ps_base::DataBuffer ToString() const;
  [[nodiscard]] PackageConfig GetPackageConfig_() const;
  void SetPackageConfig_(const PackageConfig& tpt_config);

  [[nodiscard]] const ps_base::DataBuffer& stream_id() const;
  ps_base::DataBuffer& stream_id();

  [[nodiscard]] const ps_base::DataBuffer& description() const;
  ps_base::DataBuffer& description();

  [[nodiscard]] const VideoCodecConfig& video_codec_config() const;
  VideoCodecConfig& video_codec_config();

  [[nodiscard]] const ConnectionConfig& connection_config() const;
  ConnectionConfig& connection_config();

  [[nodiscard]] const PackageConfig& package_config() const;
  PackageConfig& package_config();

  [[nodiscard]] const GraphicsBinding& graphics_binding() const;
  GraphicsBinding& graphics_binding();

  [[nodiscard]] int32_t render_width() const;
  int32_t& render_width();

  [[nodiscard]] int32_t render_height() const;
  int32_t& render_height();
  [[nodiscard]] int32_t effective_width() const;
  int32_t& effective_width();

  [[nodiscard]] int32_t effective_height() const;
  int32_t& effective_height();

 private:
  ps_base::DataBuffer stream_id_;
  ps_base::DataBuffer description_;
  VideoCodecConfig video_codec_config_;
  ConnectionConfig connection_config_;
  PackageConfig package_config_;
  GraphicsBinding graphics_binding_;
  int32_t render_width_ = 0;
  int32_t render_height_ = 0;
  int32_t effective_width_ = 0;
  int32_t effective_height_ = 0;
};
}  // namespace ps_common

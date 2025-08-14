#pragma once

#include "ps_common/codec/audio_codec_config.h"
#include "ps_common/ps_exports.h"
#include "ps_common/stream/connection_config.h"
#include "ps_common/stream/package_config.h"

namespace ps_common {
class PS_COMMON_API AudioStream {
 public:
  AudioStream() = default;
  AudioStream(const AudioStream& other);
  static const size_t kMaxStreamIdLen = 32;
  AudioStream& operator=(const AudioStream& other);
  bool operator==(const AudioStream& other);
  bool operator!=(const AudioStream& other);
  [[nodiscard]] const ps_base::DataBuffer& GetStreamId_() const;
  void SetStreamId_(const ps_base::DataBuffer& stream_id);
  void SetStreamId_(const char* stream_id);

  [[nodiscard]] AudioCodecConfig GetAudioCodecConfig_() const;
  void SetAudioCodecConfig_(const AudioCodecConfig& codec_config);

  [[nodiscard]] ConnectionConfig GetConnectionConfig_() const;
  void SetConnectionConfig_(const ConnectionConfig& conn_config);

  [[nodiscard]] PackageConfig GetTransportConfig_() const;
  void SetTransportConfig_(const PackageConfig& tpt_config);

  [[nodiscard]] const ps_base::DataBuffer& stream_id() const;
  ps_base::DataBuffer& stream_id();

  [[nodiscard]] const AudioCodecConfig& audio_codec_config() const;
  AudioCodecConfig& audio_codec_config();

  [[nodiscard]] const ConnectionConfig& connection_config() const;
  ConnectionConfig& connection_config();

  [[nodiscard]] const PackageConfig& package_config() const;
  PackageConfig& package_config();
  [[nodiscard]] ps_base::DataBuffer ToString() const;

 private:
  ps_base::DataBuffer stream_id_;
  AudioCodecConfig audio_codec_config_;
  ConnectionConfig connection_config_;
  PackageConfig package_config_;
};
}  // namespace ps_common

#pragma once
#include <cstdint>
#include <memory>

#include "ps_common/stream/audio_stream.h"
#include "ps_common/stream/edge_compression_config.h"
#include "ps_common/stream/video_stream.h"

namespace ps_common {
enum class StreamType { kUndefined = -1, kAudioStream = 0, kVideoStream = 1 };

class PS_COMMON_API StreamParam {
 public:
  StreamParam();
  StreamParam(const StreamParam& param);

  // StreamParam(StreamParam&& param) noexcept;
  explicit StreamParam(size_t vsize, size_t vcapacity, size_t asize,
                       size_t acapacity);
  ~StreamParam();
  StreamParam& operator=(const StreamParam& other);

  const VideoStream* video_stream(size_t index) const;
  VideoStream* video_stream(size_t index);
  const AudioStream* audio_stream(size_t index) const;
  AudioStream* audio_stream(size_t index);

  size_t vsize() const;
  size_t asize() const;

  void add_video_stream(const VideoStream& video_stream);
  void add_audio_stream(const AudioStream& audio_stream);

  const EdgeCompressionConfig& edge_compression() const;
  EdgeCompressionConfig& edge_compression();

 private:
  void EnsureCapacityAdequate(size_t vcapacity, size_t acapacity);

 private:
  std::unique_ptr<VideoStream[]> video_streams_;
  size_t vsize_ = 0;
  size_t vcapacity_ = 0;

  std::unique_ptr<AudioStream[]> audio_streams_;
  size_t asize_ = 0;
  size_t acapacity_ = 0;

  EdgeCompressionConfig edge_compression_config_;
};
};  // namespace ps_common

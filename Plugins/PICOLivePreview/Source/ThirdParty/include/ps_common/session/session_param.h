#pragma once
//
// Created by Administrator on 2023/3/14.
//
#include <memory>

#include "ps_base/transport/data_buffer.h"
#include "ps_common/ps_exports.h"
#include "ps_common/stream/audio_stream.h"
#include "ps_common/stream/event_stream.h"
#include "ps_common/stream/video_stream.h"

namespace ps_common {
class PS_COMMON_API SessionParam {
 public:
  typedef std::shared_ptr<SessionParam> Ptr_t;
  static Ptr_t Create();

  [[nodiscard]] virtual SessionParam* Clone() const = 0;
  virtual void Set_(const SessionParam& other) = 0;
  virtual void Reset() = 0;
  virtual bool operator==(const SessionParam& other) = 0;

  static constexpr size_t kMaxStringLen = 1024;
  [[nodiscard]] virtual const char* GetSessionId_() const = 0;
  virtual void SetSessionId_(const char* session_id) = 0;

  [[nodiscard]] virtual ConnectionType GetConnectionType_() const = 0;
  virtual void SetConnectionType_(ConnectionType val) = 0;

  [[nodiscard]] virtual const char* GetDescription_() const = 0;
  virtual void SetDescription_(const char* description) = 0;

  [[nodiscard]] virtual const AudioStream* GetAudioStream_(
      size_t index) const = 0;
  virtual AudioStream* GetAudioStream_(size_t index) = 0;
  [[nodiscard]] virtual size_t GetAudioStreamSize_() const = 0;
  virtual void AddAudioStream_(const AudioStream& audio_stream) = 0;
  virtual size_t RemoveAudioStream_(size_t index) = 0;

  [[nodiscard]] virtual const EventStream* GetEventStream_(
      size_t index) const = 0;
  virtual EventStream* GetEventStream_(size_t index) = 0;
  [[nodiscard]] virtual size_t GetEventStreamSize_() const = 0;
  virtual void AddEventStream_(const EventStream& event_stream) = 0;
  [[maybe_unused]] virtual size_t RemoveEventStream_(size_t index) = 0;

  [[nodiscard]] virtual const VideoStream* GetVideoStream_(
      size_t index) const = 0;
  virtual VideoStream* GetVideoStream_(size_t index) = 0;
  [[nodiscard]] virtual size_t GetVideoStreamSize_() const = 0;
  virtual void AddVideoStream_(const VideoStream& video_stream) = 0;
  [[maybe_unused]] virtual size_t RemoveVideoStream_(size_t index) = 0;

  [[nodiscard]] virtual ps_base::DataBuffer ToString() const = 0;

  virtual ~SessionParam() = default;
};

enum class SessionStateType {
  kIdle = 0,
  kTrying = 1,
  kReady = 2,
  kFinished = 3,
  kError = 4
};

enum class SessionRole { kUndefined = -1, kStarter = 0, kAnswer = 1 };

struct SessionState {
  SessionStateType state = SessionStateType::kIdle;
  SessionParam::Ptr_t session_param;
};
PS_COMMON_API const char* ToString(SessionStateType val);
}  // namespace ps_common
#pragma once
//
// Created by Administrator on 2023/3/29.
//
#include <cstdint>

#include "ps_base/transport/data_buffer.h"
#include "ps_common/ps_exports.h"
#include "ps_common/stream/connection_config.h"

namespace ps_common {
class PS_COMMON_API EventStream {
 public:
  EventStream() = default;
  EventStream(const EventStream& other);

  static const size_t kMaxStreamIdLen = 32;
  EventStream& operator=(const EventStream& other);
  bool operator==(const EventStream& other);
  bool operator!=(const EventStream& other);

  [[nodiscard]] const ps_base::DataBuffer& GetStreamId_() const;
  void SetStreamId_(const ps_base::DataBuffer& stream_id);
  void SetStreamId_(const char* stream_id);

  [[nodiscard]] ConnectionConfig GetConnectionConfig_() const;
  void SetConnectionConfig_(const ConnectionConfig& conn_config);
  [[nodiscard]] ps_base::DataBuffer ToString() const;

  [[nodiscard]] const ps_base::DataBuffer& stream_id() const;
  ps_base::DataBuffer& stream_id();

  [[nodiscard]] const ConnectionConfig& connection_config() const;
  ConnectionConfig& connection_config();
 private:
  ps_base::DataBuffer stream_id_;
  ps_common::ConnectionConfig conn_config_;
};
}  // namespace ps_common

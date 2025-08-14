#pragma once
#include "ps_base/ps_exports.h"
#include "ps_base/transport/data_buffer.h"
#include "ps_base/transport/network_address.h"
#include "ps_base/transport/transport_common.h"

namespace ps_base {
class PS_BASE_API TransportData {
 public:
  typedef std::shared_ptr<TransportData> Ptr_t;
  TransportData();
  TransportData(const TransportData& other);
  TransportData(TransportData&& packet);

  DataBuffer& data();
  const DataBuffer& data() const;

  // Note(by Alan Duan): Maybe return InvalidChannelId.
  TransportChannelId channel() const;
  TransportChannelId& channel();

  // Note(by Alan Duan): Maybe return invalid network address.
  NetworkAddress& remote_addr();
  const NetworkAddress& remote_addr() const;

 private:
  DataBuffer data_;
  TransportChannelId channel_id_ = kInvalidChannelId;
  NetworkAddress remote_addr_;
};
}  // namespace ps_base
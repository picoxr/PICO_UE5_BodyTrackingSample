#pragma once

#include <stdint.h>

#include <memory>

#include "ps_base/ps_exports.h"
#include "ps_base/transport/binder_info.h"
#include "ps_base/transport/local_endpoint.h"
#include "ps_base/transport/network_address.h"
#include "ps_base/transport/transport_data.h"
namespace ps_base {
class PS_BASE_API TransportClientCallback {
 public:
  virtual void NotifyConnectResult(int result) = 0;
  virtual void NotifyDisconnect(int result) = 0;
  virtual void NotifyReceiveData(const TransportData& data) = 0;

 protected:
  virtual ~TransportClientCallback() {}
};

class PS_BASE_API TransportClient {
 public:
  struct PS_BASE_API Config {
    bool enable_packet = false;
    bool enable_channel = false;
    bool enable_fd_sharing = false;
    NetworkAddress local_addr;
    NetworkAddress server_addr;
    // Used for domian socket
    LocalEndpoint local_endpoint;
#if defined(__ANDROID__)
    // Note(by Nico): Used for identify binder client and server
    BinderInfo binder_info;
#endif
    TransportClientCallback* callback = nullptr;
  };

  typedef std::shared_ptr<TransportClient> Ptr_t;

  static Ptr_t Create(TransportType type);

  virtual bool Start(const Config& config) = 0;
  virtual void Stop() = 0;
  virtual bool SendData(const TransportData& data) = 0;

  virtual TransportType type() const = 0;
  virtual const Config& config() const = 0;
  virtual bool running() const = 0;

  virtual ~TransportClient() {}
};
}  // namespace ps_base
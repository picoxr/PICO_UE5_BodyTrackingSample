#pragma once
#include <memory>

#include "ps_base/ps_exports.h"
#include "ps_base/transport/aoa/aoa_identify_info.h"
#include "ps_base/transport/local_endpoint.h"
#include "ps_base/transport/network_address.h"
#include "ps_base/transport/transport_session.h"
#include "ps_base/transport/binder_info.h"
#include "ps_base/usb/usb_device_descriptor_info.h"

namespace ps_base {
class PS_BASE_API TransportServerCallback {
 public:
  virtual void NotifyClientConnected(TransportSession::Ptr_t session) = 0;
  virtual void NotifyServerStopped(int res) = 0;

 protected:
  virtual ~TransportServerCallback() {}
};

class PS_BASE_API TransportServer {
 public:
  struct PS_BASE_API Config {
    TransportServerCallback* callback = nullptr;

    // Note(by Alan Duan): Used for tcp server.
    NetworkAddress tcp_host;

    // Note(by Alan Duan): Used for usb server or aoa server
    static const size_t kUsbDeviceMaxNum = 16;
    size_t usb_device_num = 0;
    UsbDeviceDescriptorInfo usb_devices[kUsbDeviceMaxNum];

    // Note(by Alan Duan): Used for aoa server
    AoaIdentifyInfo aoa_identify;

    // Note(by Nico): Used for msg server
    LocalEndpoint local_endpoint;

#if defined(__ANDROID__)
    // Note(by Nico): Used for identify binder client and server
    BinderInfo binder_info;
#endif

    // For file descriptor transport.
    DataBuffer file_path;
  };

  typedef std::shared_ptr<TransportServer> Ptr_t;
  static Ptr_t Create(TransportType type);

  virtual bool Start(const Config& config) = 0;
  virtual void Stop() = 0;

  virtual TransportType type() const = 0;
  virtual Config config() const = 0;
  virtual bool running() const = 0;

  virtual ~TransportServer() {}
};
}  // namespace ps_base
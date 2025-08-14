#pragma once

#include "ps_base/ps_exports.h"
#include "ps_base/transport/local_endpoint.h"
#include "ps_base/transport/network_address.h"
#include "ps_base/transport/transport_common.h"
#include "ps_base/transport/transport_data.h"
#include "ps_base/transport/usb_info.h"

namespace ps_base {
class PS_BASE_API TransportSessionCallback {
 public:
  virtual void NotifySessionStopped(int res) = 0;
  virtual void NotifyReceiveData(const TransportData& data) = 0;

 protected:
  virtual ~TransportSessionCallback() = default;
};

class PS_BASE_API TransportSession {
 public:
  typedef std::shared_ptr<TransportSession> Ptr_t;
  struct PS_BASE_API Config {
    TransportSessionCallback* callback = nullptr;

    // Add a header before data sent when theses flag opened.
    // Supported for UDP, TCP, File Descriptor and AOA transport.
    //
    //  |  channel_id(1 bytes) | packet length(4 bytes)  |
    //
    bool enable_packet = false;
    // User can get or set the channel id in TransportData.
    bool enable_channel = false;
    // For IPC: Share fd between 2 processes, using native sendmsg() and
    // recvmsg()
    bool enable_fd_sharing = false;

    // Open the udp broadcast, setting the destination port in remote_addr.
    bool udp_broadcast = false;

    // For TCP: [out] Get the client and server address from config() function.
    // For UDP: [in]  Bind the ip and port in local addr. If the remote_addr is
    //                valid, it will be used to send data.
    //          [out] Get the local and remote address from config() function.
    NetworkAddress local_addr;
    NetworkAddress remote_addr;
    // For file descriptor transport.
    DataBuffer file_path;
    // For Usb Session: [out] Represents a physical usb connection. 0 is a
    // invalid value.
    uint16_t usb_port_number = 0;
    UsbSpeed usb_speed = UsbSpeed::kUnknown;
    uint32_t send_timeout_ms = 0;
    // For domain socket transport.
    LocalEndpoint local_endpoint;
#if defined(__ANDROID__)
    // For binder transport.
    // void* binder = nullptr;
#endif
  };

  virtual bool Start(const Config& config) = 0;
  virtual void Stop() = 0;

  virtual bool SendData(const TransportData& data) = 0;

  virtual TransportType type() = 0;
  virtual Config config() = 0;
  virtual bool running() = 0;

  //Note(by Alan Duan): This function must be used on PICO Device for TCP.
  virtual bool SetTransportThreadPriority(int32_t policy, int32_t priority) = 0;

  virtual ~TransportSession() = default;
};

class PS_BASE_API TransportSessionFactory {
 public:
  struct PS_BASE_API Config {
    TransportType type = TransportType::kUndefined;
  };
  // Note(by Alan Duan): Now we only support udp and file descriptor session.
  static TransportSession::Ptr_t CreateSession(const Config& config);
};
}  // namespace ps_base
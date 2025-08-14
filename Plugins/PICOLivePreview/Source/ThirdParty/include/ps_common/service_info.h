//
// Created by Alan Duan on 2024/6/12.
//
#pragma once
#include <string>

#include "ps_common/ps_exports.h"
#include "ps_common/stream/connection_config.h"
namespace ps_common {
struct PS_COMMON_API DeviceInfo {
  std::string device_id;
  std::string device_name;
  std::string device_os_type;
  std::string device_version;

  [[nodiscard]] std::string ToString() const;
  bool operator==(const DeviceInfo& other) const;
  bool operator!=(const DeviceInfo& other) const;
  bool operator<(const DeviceInfo& other) const;
};
enum class StreamingServiceType { kMRDevice = 0, kMRExternal = 1 };
struct PS_COMMON_API ServiceInfo {
  StreamingServiceType service_type = StreamingServiceType::kMRDevice;
  std::string service_id;
  std::string service_name;
  std::string service_version;

  [[nodiscard]] std::string ToString() const;
  bool operator==(const ServiceInfo& other) const;
  bool operator!=(const ServiceInfo& other) const;
  bool operator<(const ServiceInfo& other) const;
};
struct PS_COMMON_API ConnectionInfo {
  std::string connection_id;
  ps_common::ConnectionType connection_type =
      ps_common::ConnectionType::kUndefined;
  DeviceInfo peer_device_info;
  ServiceInfo peer_service_info;
  bool compatible_flag = false;

  [[nodiscard]] std::string ToString() const;
  bool operator==(const ConnectionInfo& other) const;
  bool operator!=(const ConnectionInfo& other) const;
  bool operator<(const ConnectionInfo& other) const;
};
}  // namespace ps_common

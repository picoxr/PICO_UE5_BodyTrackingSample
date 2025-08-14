#pragma once

#include <stdint.h>

#include <memory>

#include "ps_base/transport/network_address.h"
#include "ps_common/ps_exports.h"

namespace ps_common {
enum class ConnectionType {
  kUndefined = 0,
  kUdp,
  kTcp,
  kUsb,
};

enum class Direction { kUndefined = 0, kSend, kReceive, kSendAndReceive };
PS_COMMON_API Direction ReverseDirection(Direction val);
enum class SetupRole { kClient = 0, kServer = 1, kBoth = 2 };

class PS_COMMON_API ConnectionConfig {
 public:
  ConnectionConfig();
  ConnectionConfig(const ConnectionConfig& config);
  bool operator==(const ConnectionConfig& other);
  bool operator!=(const ConnectionConfig& other);
  [[nodiscard]] ConnectionType GetConnectionType_() const;
  void SetConnectionType_(ConnectionType type);

  [[nodiscard]] ps_base::NetworkAddress GetAddress_() const;
  void SetAddress_(const ps_base::NetworkAddress& addr);

  [[nodiscard]] Direction GetDirection_() const;
  void SetDirection_(Direction drt);

  [[nodiscard]] ConnectionType connection_type() const;
  ConnectionType& connection_type();

  [[nodiscard]] const ps_base::NetworkAddress& addr() const;
  ps_base::NetworkAddress& addr();

  [[nodiscard]] Direction direction() const;
  Direction& direction();

  [[nodiscard]] SetupRole role() const;
  SetupRole& role();

  [[nodiscard]] uint32_t channel() const;
  uint32_t& channel();

  ConnectionConfig& operator=(const ConnectionConfig& other);
  [[nodiscard]] ps_base::DataBuffer ToString() const;

 private:
  ConnectionType connection_type_ = ConnectionType::kUndefined;
  ps_base::NetworkAddress addr_;
  Direction direction_ = Direction::kUndefined;
  SetupRole role_ = SetupRole::kBoth;
  uint32_t channel_ = 0;
};
PS_COMMON_API const char* ToString(ConnectionType val);
};  // namespace ps_common

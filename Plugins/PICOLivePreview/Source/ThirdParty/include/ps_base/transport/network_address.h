#pragma once
#include <stdint.h>

#include "ps_base/ps_exports.h"
#include "ps_base/transport/data_buffer.h"

namespace ps_base {
class PS_BASE_API NetworkAddress {
 public:
  NetworkAddress();
  NetworkAddress(const char* host, uint16_t port);
  bool operator==(const NetworkAddress& other) const;
  // Todo(by Alan Duan): We should check if the string is valid.
  const DataBuffer& host() const;
  DataBuffer& host();

  uint16_t& port();
  uint16_t port() const;
  // Note(by Alan Duan): Always return string or nullptr.
  const char* Host_() const;
  size_t HostLen_() const;
  void SetHost_(const char* val);

  uint16_t Port_() const;
  void SetPort_(uint16_t val);
  void Set_(const char* host, uint16_t port);
  bool IsValid() const;

 private:
  DataBuffer host_;
  uint16_t port_ = 0;
};
}  // namespace ps_base
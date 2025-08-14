#pragma once
#include <stdint.h>

#include "ps_base/ps_exports.h"
#include "ps_base/transport/data_buffer.h"

namespace ps_base {
class PS_BASE_API LocalEndpoint {
 public:
  LocalEndpoint();
  LocalEndpoint(const char* domain);
  bool operator==(const LocalEndpoint& other) const;

  const DataBuffer& domain() const;
  DataBuffer& domain();

  const char* Domain_() const;
  size_t DomainLen_() const;
  void SetDomain_(const char* val);

  void Set_(const char* domain);
  bool IsValid() const;

 private:
  DataBuffer domain_;
};
}  // namespace ps_base
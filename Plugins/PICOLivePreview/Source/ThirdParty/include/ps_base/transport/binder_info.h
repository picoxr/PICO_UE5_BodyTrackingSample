#pragma once
#include <stdint.h>

#include "ps_base/ps_exports.h"
#include "ps_base/transport/data_buffer.h"

namespace ps_base {
class PS_BASE_API BinderInfo {
 public:
  BinderInfo();
  BinderInfo(const char* id, void* binder);
  bool operator==(const BinderInfo& other) const;

  const DataBuffer& id() const;
  DataBuffer& id();

  const char* Id_() const;
  size_t IdLen_() const;
  void SetId_(const char* val);

  void* Binder();
  void SetBinder(void* binder);

  void Set_(const char* id, void* binder);
  bool IsValid() const;

 private:
  DataBuffer id_;
  void* binder_ = nullptr;
};
}  // namespace ps_base
#pragma once

#if defined(__ANDROID__)
#include "ps_base/ps_exports.h"
#include "ps_base/transport/data_buffer.h"
namespace ps_base {
class PS_BASE_API AndroidBinderTransporterCallback {
 public:
  virtual void NotifyReceiveData(const DataBuffer& data) = 0;

 protected:
  virtual ~AndroidBinderTransporterCallback() {}
};
class PS_BASE_API AndroidBinderTransporter {
 public:
  typedef std::shared_ptr<AndroidBinderTransporter> Ptr_t;

  // struct PS_BASE_API Config {
  //   AndroidBinderTransporterCallback* callback = nullptr;
  // };
  virtual bool Initialize() = 0;
  virtual bool Alive() = 0;
  virtual bool Send(const DataBuffer& data) = 0;
  virtual void SetCallback(AndroidBinderTransporterCallback* callback) = 0;
  virtual ~AndroidBinderTransporter() = default;
};

}  // namespace ps_base

#endif
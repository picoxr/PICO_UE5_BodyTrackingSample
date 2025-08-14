#pragma once

#if defined(__ANDROID__)
#include <android/binder_auto_utils.h>

#include "ps_base/binder/aidl/android_binder_transporter.h"
#include "ps_base/ps_exports.h"
namespace ps_base {
class PS_BASE_API AndroidTransportBinderServiceCallback {
 public:
  virtual void NotifyTransporterConnected(
      const char* id, AndroidBinderTransporter::Ptr_t transporter) = 0;
  virtual void NotifyServiceStopped(int res) = 0;

 protected:
  virtual ~AndroidTransportBinderServiceCallback() {}
};
class PS_BASE_API AndroidTransportBinderService {
 public:
  typedef std::shared_ptr<AndroidTransportBinderService> Ptr_t;

  static Ptr_t Instance();
  virtual bool Initialize() = 0;
  virtual ::ndk::SpAIBinder AsBinder() = 0;
  virtual void AddOrUpdateCallback(
      const char* id, AndroidTransportBinderServiceCallback* callback) = 0;
  virtual ~AndroidTransportBinderService() = default;
};

}  // namespace ps_base

#endif
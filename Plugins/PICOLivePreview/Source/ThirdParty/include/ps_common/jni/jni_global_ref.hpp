#pragma once

#if defined(__ANDROID__)

#include "ps_common/jni/jni_utils.h"
#include "ps_common/jni/jni_local_ref.hpp"

namespace ps_common {

template <typename JniType>
class PS_COMMON_API JniGlobalRef {
 public:
  JniGlobalRef() : obj_(nullptr) {}
  JniGlobalRef(const JniGlobalRef<JniType> &ref) : obj_(nullptr) {
    Set(ref.Get());
  }
  JniGlobalRef(const JniLocalRef<JniType> &ref) : obj_(nullptr) { Set(ref.Get()); }

  ~JniGlobalRef() { Set(nullptr); }

  JniType Get() const { return obj_; }

  void Set(JniType obj) {
    JNIEnv *env = nullptr;
    if (obj_ || obj) {
      env = JniUtils::GetEnvForCurrentThread();
    }
    if (obj_) {
      if (env) {
        env->DeleteGlobalRef(obj_);
      }
      obj_ = nullptr;
    }
    if (obj && env) {
      obj_ = (JniType)env->NewGlobalRef(obj);
    }
  }

  operator JniType() const { return obj_; }

  void operator=(const JniLocalRef<JniType> &ref) { Set(ref.Get()); }

 private:
  JniType obj_;
};

}  // namespace ps_common

#endif


#pragma once

#if defined(__ANDROID__)


#include "ps_common/jni/jni_utils.h"

namespace ps_common {

template <typename JniType>
class PS_COMMON_API JniLocalRef {
 public:
  JniLocalRef() : obj_(nullptr) {}
  JniLocalRef(JniType obj) : obj_(nullptr) { Set(obj); }
  JniLocalRef(const JniLocalRef<JniType> &ref) : obj_(nullptr) {
    if (ref.Get()) {
      JNIEnv *env = JniUtils::GetEnvForCurrentThread();
      Set(env ? (JniType)env->NewLocalRef(ref.Get()) : nullptr);
    }
  }

  ~JniLocalRef() { Set(nullptr); }

  JniType Get() const { return obj_; }
  void Set(JniType obj) {
    if (obj_) {
      JNIEnv *env = JniUtils::GetEnvForCurrentThread();
      if (env) env->DeleteLocalRef(obj_);
    }
    obj_ = obj;
  }

  JniType Leak() {
    JniType obj = obj_;
    obj_ = nullptr;
    return obj;
  }

  operator JniType() const { return obj_; }

  void operator=(JniType obj) { Set(obj); }
  void operator=(const JniLocalRef<JniType> &ref) {
    if (ref.Get()) {
      JNIEnv *env = JniUtils::GetEnvForCurrentThread();
      Set(env ? (JniType)env->NewLocalRef(ref.Get()) : nullptr);
    } else {
      Set(nullptr);
    }
  }

 private:
  JniType obj_;
};

}  // namespace ps_common
#endif
#pragma once

/*
// Only support on Andorid now
*/

#if defined(__ANDROID__)

#ifndef PS_JNI_VERSION
#define PS_JNI_VERSION JNI_VERSION_1_6
#endif

#include <jni.h>

#include <string>

#include "ps_common/ps_exports.h"

namespace ps_common {

class JniUtils {
 private:
  JniUtils() {}
  JniUtils(const JniUtils&) {}
  virtual ~JniUtils() {}

 public:
  static PS_COMMON_API JNIEnv* Initialize(JavaVM* jvm);
  static PS_COMMON_API bool IsInitialized();
  static PS_COMMON_API JavaVM* GetJavaVM();

  static PS_COMMON_API JNIEnv* GetEnvForCurrentThread();
  static PS_COMMON_API JNIEnv* AttachCurrentThreadToJVM(bool& attached);
  static PS_COMMON_API JNIEnv* GetEnvForCurrentThread(JavaVM* jvm);
  static PS_COMMON_API JNIEnv* AttachCurrentThreadToJVM(
      const char* thread_name);
  static PS_COMMON_API JNIEnv* AttachCurrentThreadIfNeeded();
  static PS_COMMON_API JNIEnv* AttachCurrentThreadAsDaemonToJVM(
      const char* thread_name);
  static PS_COMMON_API void DetachCurrentThreadFromJVM();
  static PS_COMMON_API std::string GetThreadId();
  static PS_COMMON_API std::string GetThreadName();
};

}  // namespace ps_common

#endif
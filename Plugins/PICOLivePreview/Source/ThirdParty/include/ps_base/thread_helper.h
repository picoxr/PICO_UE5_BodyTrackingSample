#pragma once
//
// Created by Administrator on 2023/6/5.
//
#include <cstdint>

#include "ps_base/ps_exports.h"
namespace ps_base {
class PS_BASE_API ThreadHelper {
 public:
  static void sleep(uint64_t ms);

  // Note(by Alan Duan&Yutong): The thread name length must be less than 16
  //                     on Android and 64 for Windows and Mac.
  static void SetThisThreadName(const char* name);
  static char* GetThisThreadName();
  // Note(by Alan Duan): Thread native handle has different type
  //                     on different platform.
  //                     Android: pthread_t => long
  //                     Windows: HANDLE => void*
  //                     Mac    : <not supported>
  //                     Example:
  //                       std::thread t;
  //                       SetThisThreadName((int64_t)t.native_handle(),
  //                       "TestThread");
  static void SetThreadName(int64_t native_handle, const char* name);

  static void InjectCrash();

  static bool SetCurrentThreadPriority(int32_t policy, int32_t priority);
};
}  // namespace ps_base
#pragma once
// Auth: Alan Duan
// Date: 2021-11-11
// Desp: Log output function.

#include <stdint.h>

#include <memory>
#include <sstream>

#include "ps_base/ps_exports.h"

namespace ps_base {
enum class LogLevel {
  kLogLevelInvalid = -1,
  kLogLevelDebug = 0,
  kLogLevelInfo = 1,
  kLogLevelWarn = 2,
  kLogLevelError = 3
};
//
// brief: Log message wrapper. Should not be used directly.
//
class PS_BASE_API LogMessage {
 public:
  static std::shared_ptr<LogMessage> Create(const std::string& file, long line,
                                            LogLevel type,
                                            const std::string& func = "");
  virtual std::stringstream& stream() = 0;
  virtual LogLevel level() const = 0;
  static std::string PointerToString(void* p);

 protected:
  virtual ~LogMessage(){};
};

class PS_BASE_API LogWriter {
 public:
  typedef void (*CustomLogFunctionPtr)(LogMessage&);
  //
  // brief: Set the log file path.
  //       Must be called before writing the first log message.
  // argv : [in] name: The log file path.
  //
  static void SetLogFilePrefix(const std::string& name);
  //
  // brief: Set the log min output level.
  //       e.g. If setting kLogLevelInfo, debug message cannot be output.
  //       Should be set before writing the first log message.
  // argv : [in] level: The log min output level.
  //       The default value is kLogLevelInfo.
  //
  static void SetOuputLogLevel(LogLevel level);

  static void SetCustomLogFunction(CustomLogFunctionPtr log_func);

  //
  // brief: Should not be called directly!!!
  //       Use the macro instead.
  // argv : [in] log_msg: The message need to be output.
  //
  static void Log(LogMessage& log_msg);

#if defined(__ANDROID__)
  static void SetLogTag(const std::string& tag);
#endif
};
}  // namespace ps_base
//
// brief: Convert pointer to hexadecimal, just like 0xefdffa70.
//
#define PS_LOG_PTR(ptr) ps_base::LogMessage::PointerToString((void*)ptr)
//
// brief: Should not be used. Tool Macro to output this pointer.
//
#define PS_THIS_PTR PS_LOG_PTR(this)
//
// brief: Should not be used. Tool Macro.
//
#define PS_LOG(msg, type)                                           \
  do {                                                              \
    auto m = ps_base::LogMessage::Create(                           \
        __FILE__, __LINE__, ps_base::LogLevel::type, __FUNCTION__); \
    m->stream() << msg;                                             \
    ps_base::LogWriter::Log(*m);                                    \
  } while (false);
#define PS_LOG_THIS(msg, type) PS_LOG(msg << ", this= " << PS_THIS_PTR, type);
//
// brief: Output log with different level.
//       E.x. PS_DEBUG("my value= " << value << ", second value= " << value2);
//       Only support base type.
//
#define PS_DEBUG(msg) PS_LOG(msg, kLogLevelDebug)
#define PS_INFO(msg) PS_LOG(msg, kLogLevelInfo)
#define PS_WARN(msg) PS_LOG(msg, kLogLevelWarn)
#define PS_ERROR(msg) PS_LOG(msg, kLogLevelError)

//
// brief: Output log with this pointer. Should be used in class member function,
//       not static function.
//
#define PS_DEBUG_THIS(msg) PS_LOG_THIS(msg, kLogLevelDebug)
#define PS_INFO_THIS(msg) PS_LOG_THIS(msg, kLogLevelInfo)
#define PS_WARN_THIS(msg) PS_LOG_THIS(msg, kLogLevelWarn)
#define PS_ERROR_THIS(msg) PS_LOG_THIS(msg, kLogLevelError)

//
// brief: Check the condition and output error log.
//       E.x. PS_CHECK(p == nullptr, "invalid pointer");
//
// NOTE(by Alan Duan): Lack of operator checking, use carefully.
#define PS_CHECK(C, S)                       \
  {                                          \
    if (C) {                                 \
      PS_ERROR_THIS(#S << ", cond= " << #C); \
    }                                        \
  }
//
// brief: Check the condition, output error log and return value.
//       E.x. PS_CHECK_RET(p == nullptr, "invalid pointer", false);
//
#define PS_CHECK_RET(C, S, V)          \
  {                                    \
    if (C) {                           \
      PS_ERROR_THIS(#S << ", " << #C); \
      return V;                        \
    }                                  \
  }

//
// brief: Check the condition, output error log and run the code.
//       E.x. PS_CHECK_RET(p == nullptr, "invalid pointer", continue);
//
#ifndef PS_CHECK_RUN
#define PS_CHECK_RUN(C, S, R)          \
  {                                    \
    if (C) {                           \
      PS_ERROR_THIS(#S << ", " << #C); \
      R;                               \
    }                                  \
  }
#endif

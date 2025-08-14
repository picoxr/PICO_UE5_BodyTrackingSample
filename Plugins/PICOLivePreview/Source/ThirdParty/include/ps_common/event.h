#pragma once
//
// Created by Administrator on 2023/8/9.
//
#include "ps_common/ps_exports.h"
#include <memory>
#include <string>
namespace ps_common {
class PS_COMMON_API Event {
 public:
  typedef std::shared_ptr<Event> Ptr_t;
  static Ptr_t Create(std::string event_type);
  virtual std::string GetEventType() const = 0;
  virtual std::string GetContentsString() const = 0;
  virtual void SetContentString(const std::string& str) = 0;
  virtual std::string ToString() const = 0;

 protected:
  virtual ~Event() = default;
};
}  // namespace ps_common

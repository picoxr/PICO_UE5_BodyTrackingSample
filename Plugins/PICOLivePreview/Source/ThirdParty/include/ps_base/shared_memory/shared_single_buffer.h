#pragma once
//
// Created by Alan Duan on 2024/3/14.
//
#include <memory>

#include "ps_base/ps_exports.h"
#include "ps_base/transport/data_buffer.h"

namespace ps_base {
class PS_BASE_API SharedSingleBuffer {
 public:
  typedef std::shared_ptr<SharedSingleBuffer> Ptr_t;
  static Ptr_t Create();
  virtual int Init(const char* name, int size) = 0;
  virtual void Reset() = 0;
  virtual int Write(const char* data, int len) = 0;
  virtual int Write(int offset, const char* data, int len) = 0;
  virtual int Read(char* buffer, int buffer_len) = 0;
  virtual int Read(int offset, char* buffer, int buffer_len) = 0;
  ~SharedSingleBuffer() = default;
};
}  // namespace ps_base

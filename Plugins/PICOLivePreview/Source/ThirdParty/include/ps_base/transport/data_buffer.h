#pragma once
#include <stdint.h>

#include <memory>

#include "ps_base/ps_exports.h"
namespace ps_base {
// Note(by Alan Duan): Attention!!! If not expected to copy buffer,
//                     Please use std::move to construct new DataBuffer,
//                     just like this - DataBuffer data(std::move(old_data));
class PS_BASE_API DataBuffer {
 public:
  typedef std::shared_ptr<DataBuffer> Ptr_t;
  DataBuffer();
  DataBuffer(const DataBuffer&);
  DataBuffer& operator=(const DataBuffer&);

  DataBuffer(DataBuffer&& buf) noexcept;
  DataBuffer(const uint8_t* data, size_t size, size_t capacity);
  explicit DataBuffer(size_t size);
  explicit DataBuffer(size_t size, size_t capacity);
  explicit DataBuffer(const uint8_t* data, size_t size);

  uint8_t* Data_();
  [[nodiscard]] const uint8_t* Data_() const;
  [[nodiscard]] bool Empty() const;
  [[nodiscard]] size_t Offset_() const;
  [[nodiscard]] size_t Size_() const;
  [[nodiscard]] size_t Capacity_() const;

  DataBuffer& operator=(DataBuffer&& buf) noexcept;
  bool operator==(const DataBuffer& buf) const;
  bool operator!=(const DataBuffer& buf) const;
  uint8_t& operator[](size_t index);
  uint8_t operator[](size_t index) const;

  uint8_t* begin();
  uint8_t* end();
  [[nodiscard]] const uint8_t* begin() const;
  [[nodiscard]] const uint8_t* end() const;
  [[nodiscard]] const uint8_t* cbegin() const;
  [[nodiscard]] const uint8_t* cend() const;

  void SetData(const uint8_t* data, size_t size);
  // void SetData(const uint8_t(&array)[]);

  void AppendData(const uint8_t* data, size_t size);
  void AppendData(const uint8_t& data);
  void SetOffset_(size_t offset, size_t size);
  void SetSize_(size_t size);
  void EnsureCapacity(size_t capacity);
  void Clear();
  [[nodiscard]] const char* ToString() const;
  friend void swap(DataBuffer& a, DataBuffer& b);
  // void AppendData(const uint8_t(&array)[]);
 private:
  [[nodiscard]] bool IsConsistent() const;
  void OnMovedFrom();
  void EnsureCapacityWithHeadroom(size_t capacity, bool extra_headroom);

 private:
  size_t offset_ = 0;
  size_t size_ = 0;
  size_t capacity_ = 0;
  std::unique_ptr<uint8_t[]> data_;
};
}  // namespace ps_base
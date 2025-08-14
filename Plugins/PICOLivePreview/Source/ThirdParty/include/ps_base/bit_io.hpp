//
// Created by Nico on 2024/3/26.
//

#pragma once

#include <bitset>
#include <cstdint>
#include <cstdlib>
namespace ps_base {
template <typename T>
class BitReader {
 private:
  size_t size_ = sizeof(T) * 8;
  std::bitset<sizeof(T) * 8> set;

 public:
  void Set(T bytes) {
    set = std::bitset<sizeof(T) * 8>(bytes);
    size_ = sizeof(T) * 8;
  }

  // bit_pos: from 0;
  // n: read num of bit
  // returns: sum of n bits from bit_pos, -1 when error;
  int64_t Read(size_t bit_pos, size_t n = 1) {
    if ((bit_pos + n) >= size_) {
      return -1;
    }
    int64_t res = 0;
    for (size_t i = bit_pos; i < bit_pos + n; i++) {
      res = res * 2 + set[size_ - 1 - i];
    }
    return res;
  }

 private:
  bool GetBit(T x, size_t pos) { return (x >> (size_ - pos)) & 1; }
};
}  // namespace ps_base
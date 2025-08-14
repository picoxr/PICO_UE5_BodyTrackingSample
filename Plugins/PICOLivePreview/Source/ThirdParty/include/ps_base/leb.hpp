#pragma once

#include <cstdint>
#include <cstdlib>

#include "ps_base/transport/data_buffer_reader.h"

namespace ps_base {
/**
 * Read a unsigned integer coded as a variable number of up to eight
 * little-endian bytes, where the MSB in a byte signals another byte
 * must be read.
 * All coded bits are read, but values > UINT_MAX are truncated.
 */
static inline uint8_t Leb(DataBufferReader& reader) {
  int32_t more, i = 0;
  uint8_t leb = 0;

  do {
    uint8_t byte = 0;
    reader.Read(&byte, 1);
    uint8_t bits = byte & 0x7f;
    more = byte & 0x80;
    if (i <= 4) leb |= bits << (i * 7);
    if (++i == 8) break;
  } while (more);

  return leb;
}

/**
 * Read a unsigned integer coded as a variable number of up to eight
 * little-endian bytes, where the MSB in a byte signals another byte
 * must be read.
 */
static inline int64_t Leb128(DataBufferReader& reader) {
  int64_t ret = 0;

  for (int i = 0; i < 8; i++) {
    uint8_t byte = 0;
    reader.Read(&byte, 1);
    ret |= (int64_t)(byte & 0x7f) << (i * 7);
    if (!(byte & 0x80)) break;
  }

  return ret;
}
}  // namespace ps_base
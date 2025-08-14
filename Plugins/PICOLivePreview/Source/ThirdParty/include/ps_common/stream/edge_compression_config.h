#pragma once
#include <cstddef>
#include <cstdint>

#include "ps_common/ps_exports.h"

namespace ps_common {
class PS_COMMON_API EdgeCompressionConfig {
 public:
  EdgeCompressionConfig();
  ~EdgeCompressionConfig();
  EdgeCompressionConfig(const EdgeCompressionConfig& other);
  EdgeCompressionConfig(EdgeCompressionConfig&& other) noexcept;
  EdgeCompressionConfig& operator=(const EdgeCompressionConfig& other);
  bool operator==(const EdgeCompressionConfig& other);
  size_t original_width() const;
  size_t& original_width();

  size_t original_height() const;
  size_t& original_height();

  size_t edge_width() const;
  size_t& edge_width();

  size_t edge_height() const;
  size_t& edge_height();

  size_t compressed_width() const;
  size_t compressed_height() const;

  float compression() const;
  float& compression();

 private:
  size_t original_width_ = 0;
  size_t original_height_ = 0;
  size_t edge_width_ = 0;
  size_t edge_height_ = 0;
  float compression_ = 0.0f;
};
}  // namespace ps_common
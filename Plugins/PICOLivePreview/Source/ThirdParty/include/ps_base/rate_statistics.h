#pragma once
//
// Created by Alan Duan on 2023/10/17.
//
#include <cstdint>
#include <deque>
#include "ps_base/ps_exports.h"
namespace ps_base {
class PS_BASE_API RateStatistics {
 public:
  RateStatistics(int64_t max_window_size_ms, float scale);
  RateStatistics(const RateStatistics& other);
  RateStatistics(RateStatistics&& other) noexcept ;
  ~RateStatistics();
  void Reset();
  void Update(int64_t count, int64_t now_ms);
  [[nodiscard]] int64_t Rate(int64_t now_ms) const;
  bool SetWindowSize(int64_t window_size_ms, int64_t now_ms);

 private:
  void EraseOld(int64_t now_ms);

  struct Bucket {
    explicit Bucket(int64_t timestamp);
    int64_t sum;
    int num_samples;
    const int64_t timestamp;
  };
  std::deque<Bucket> buckets_;
  int64_t accumulated_count_;
  int64_t first_timestamp_;
  bool overflow_ = false;
  int num_samples_;
  const float scale_;
  const int64_t max_window_size_ms_;
  int64_t current_window_size_ms_;
};
}  // namespace ps_base

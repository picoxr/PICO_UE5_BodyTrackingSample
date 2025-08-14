#pragma once

#include <stdint.h>

#include "ps_common/ps_exports.h"

namespace ps_common {

class PS_COMMON_API HmdStatisticsData {
 public:
  uint64_t GetTimestampMs_() const;
  void SetTimestampMs_(uint64_t time_stamp_ms);

  uint64_t GetAverageTransferTime_(int64_t index) const;
  void SetAverageTransferTime_(int64_t index, uint64_t aver_trans_time);

  uint64_t GetAverageDecodeTime_(int64_t index) const;
  void SetAverageDecodeTime_(int64_t index, uint64_t aver_dec_time);

  float GetPacketLoss_(int64_t index) const;
  void SetPacketLoss_(int64_t index, float pack_loss);

  void Reset();

 private:
  uint64_t time_stamp_ms_ = 0;
  uint64_t average_transfer_time_[2] = {0, 0};
  uint64_t average_decode_time_[2] = {0, 0};
  float packet_loss_[2] = {0, 0};
};
}  // namespace ps_common
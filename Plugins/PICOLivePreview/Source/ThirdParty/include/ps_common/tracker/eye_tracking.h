#pragma once
//
// Created by Administrator on 2023/9/14.
//
#include <cstdint>

#include "ps_base/transport/data_buffer.h"
#include "ps_common/ps_exports.h"
#include "ps_common/quaternion.h"
#include "ps_common/vector.h"

namespace ps_common {

struct EyeTrackingDataKHR {
  Vector position;
  Quaternion rotation;
};

/** combined means center-hmd point*/
struct EyeTrackingDataPICO {
  //!< Bit field indicating eye pose status
  int32_t left_eye_pose_status = 0;
  int32_t right_eye_pose_status = 0;
  int32_t combined_eye_pose_status = 0;

  /** unit:mm */
  Vector left_eye_gaze_position;
  Vector right_eye_gaze_position;
  Vector combined_eye_gaze_position;

  /** unit:meter */
  Vector left_eye_gaze_vector;
  Vector right_eye_gaze_vector;
  Vector combined_eye_gaze_vector;

  /** 0-1 */
  float left_eye_openness = 0;
  float right_eye_openness = 0;
  float left_eye_pupil_dilation = 0;
  float right_eye_pupil_dilation = 0;

  /**
   * inner eye corner's position in pic
   * unit:meter */
  Vector left_inner_eye_corner_position;
  Vector right_inner_eye_corner_position;
  Vector foveated_gaze_direction;

  /** The current state of the foveatedGazeDirection signal.*/
  int32_t foveated_gaze_tracking_state{};
};

class PS_COMMON_API EyeTrackingData {
 public:
  EyeTrackingData() = default;
  EyeTrackingData(const EyeTrackingData& other) = default;
  ~EyeTrackingData() = default;
  EyeTrackingData& operator=(const EyeTrackingData& other) = default;
  void Reset();
  [[nodiscard]] ps_base::DataBuffer ToString() const;

  [[nodiscard]] bool et_khr_available() const { return et_khr_available_; }
  bool& mutable_et_khr_available() { return et_khr_available_; }
  [[nodiscard]] EyeTrackingDataKHR et_khr() const {
    return eye_tracking_data_khr_;
  }
  EyeTrackingDataKHR& mutable_et_khr() { return eye_tracking_data_khr_; }

  [[nodiscard]] bool et_pico_available() const { return et_pico_available_; }
  bool& mutable_et_pico_available() { return et_pico_available_; }
  [[nodiscard]] EyeTrackingDataPICO et_pico() const {
    return eye_tracking_data_pico_;
  }
  EyeTrackingDataPICO& mutable_et_pico() { return eye_tracking_data_pico_; }

  [[nodiscard]] int64_t timestamp_ns() const { return timestamp_ns_; }
  int64_t& mutable_timestamp_ns() { return timestamp_ns_; }

 private:
  struct EyeTrackingDataKHR eye_tracking_data_khr_;
  struct EyeTrackingDataPICO eye_tracking_data_pico_;

  bool et_khr_available_ = false;
  bool et_pico_available_ = false;
  int64_t timestamp_ns_ = 0;
};

}  // namespace ps_common

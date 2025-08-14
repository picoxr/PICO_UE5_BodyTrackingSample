#pragma once
//
// Created by Administrator on 2023/6/20.
//
#include <cstdint>

#include "ps_common/device_type.h"
#include "ps_common/pose.h"
namespace ps_common {
enum class HandJointType {
  kPalmExt = 0,
  kWristExt = 1,
  kThumbMetacarpalExt = 2,
  kThumbProximalExt = 3,
  kThumbDistalExt = 4,
  kThumbTipExt = 5,
  kIndexMetacarpalExt = 6,
  kIndexProximalExt = 7,
  kIndexIntermediateExt = 8,
  kIndexDistalExt = 9,
  kIndexTipExt = 10,
  kMiddleMetacarpalExt = 11,
  kMiddleProximalExt = 12,
  kMiddleIntermediateExt = 13,
  kMiddleDistalExt = 14,
  kMiddleTipExt = 15,
  kRingMetacarpalExt = 16,
  kRingProximalExt = 17,
  kRingIntermediateExt = 18,
  kRingDistalExt = 19,
  kRingTipExt = 20,
  kLittleMetacarpalExt = 21,
  kLittleProximalExt = 22,
  kLittleIntermediateExt = 23,
  kLittleDistalExt = 24,
  kLittleTipExt = 25
};
static const int32_t kMaxHandJointCount = 32;
struct HandJointLocation {
  uint64_t location_flags = 0;
  Pose pose;
  float radius = 0.0f;
};
struct HandJointVelocity {
  uint64_t velocity_flags = 0;
  Vector linear_velocity;
  Vector angular_velocity;
};
struct HandJointVelocities {
  uint32_t joint_count = 0;
  HandJointVelocity joint_velocities[kMaxHandJointCount];
};
struct HandAimState {
  // match struct XrHandTrackingAimStateFB extends XrHandJointLocationsEXT
  uint64_t status = 0;
  Pose aim_pose;
  float pinch_strength_index = 0.0f;
  float pinch_strength_middle = 0.0f;
  float pinch_strength_ring = 0.0f;
  float pinch_strength_little = 0.0f;
  // PICO private extension
  float click_strength = 0.0f;
};
struct HandJointLocations {
  int64_t timestamp_ns = 0;
  SideType side_type = SideType::kUndefined;
  bool is_active = false;
  uint32_t joint_count = 0;
  HandJointLocation joint_locations[kMaxHandJointCount];
  HandJointVelocities joint_velocities;
  HandAimState aim_state;
};
}  // namespace ps_common

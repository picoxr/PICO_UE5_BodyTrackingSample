#pragma once
//
// Created by Administrator on 2023/9/14.
//
#include <cstdint>

#include "ps_base/transport/data_buffer.h"
#include "ps_common/pose.h"
#include "ps_common/ps_exports.h"

namespace ps_common {
enum class BodyJointType {
  kNoneRole = -1,
  kPelvis = 0,
  kLeftHip = 1,
  kRightHip = 2,
  kSpine1 = 3,
  kLeftKnee = 4,
  kRightKnee = 5,
  kSpine2 = 6,
  kLeftAnkle = 7,
  kRightAnkle = 8,
  kSpine3 = 9,
  kLeftFoot = 10,
  kRightFoot = 11,
  kNeck = 12,
  kLeftCollar = 13,
  kRightCollar = 14,
  kHead = 15,
  kLeftShoulder = 16,
  kRightShoulder = 17,
  kLeftElbow = 18,
  kRightElbow = 19,
  kLeftWrist = 20,
  kRightWrist = 21,
  kLeftHand = 22,
  kRightHand = 23,
  kCount = 24
};
static const int32_t kMaxBodyJointCount = 24;

enum class BodyJointSet {
  kDefault = 0,
  kBodyStarWithoutArm = 1,
  kBodyFullStar = 2
};
struct BodyJointLocation {
  uint64_t location_flags = 0;
  Pose pose;
  float radius = 0.0f;
};
struct BodyJointVelocity {
  uint64_t velocity_flags = 0;
  Vector linear_velocity;
  Vector angular_velocity;
};
struct BodyJointVelocities {
  uint32_t joint_count = 0;
  BodyJointVelocity joint_velocities[kMaxBodyJointCount];
};
struct BodyJointAcceleration {
  uint64_t acceleration_flags = 0;
  Vector linear_acceleration;
  Vector angular_acceleration;
};
struct BodyJointAccelerations {
  uint32_t joint_count = 0;
  BodyJointAcceleration joint_accelerations[kMaxBodyJointCount];
};
struct BodyJointPostureFlags {
  uint32_t joint_count = 0;
  uint32_t posture_flags[kMaxBodyJointCount]{};
};
struct BodyJointGlobalPose {
  uint32_t joint_count = 0;
  Pose global_joint_locations[kMaxBodyJointCount];
};
struct BodyJointLocations {
  int64_t timestamp_ns = 0;
  bool is_active = false;
  uint32_t joint_count = 0;
  BodyJointLocation joint_locations[kMaxBodyJointCount];
  BodyJointVelocities joint_velocities;
  BodyJointAccelerations joint_accelerations;
  BodyJointPostureFlags posture_flags;
  BodyJointGlobalPose global_pose;
};
enum class BodyTrackingStatusCode { kInvalid = 0, kValid = 1, kLimited = 2 };
enum class BodyTrackingErrorCode {
  kInnerException = 0,
  kNotCalibrated = 1,
  kNumNotEnough = 2,
  kStateNotSatisfied = 3,
  kPersistentInvisibility = 4,
  kDataError = 5,
  kUserChange = 6,
  kTrackingPoseError = 7
};
struct ConnectedBodyTrackerPICO {
  uint8_t size = 0;
  uint8_t tracker_id[12]{0};
};
struct BodyTrackingBoneLength {
  float head = 0.0f;
  float neck = 0.0f;
  float torso = 0.0f;
  float hip = 0.0f;
  float upper_leg = 0.0f;
  float lower_leg = 0.0f;
  float foot = 0.0f;
  float shoulder = 0.0f;
  float upper_arm = 0.0f;
  float lower_arm = 0.0f;
  float hand = 0.0f;
};
struct BodyTrackingAlgParam {
  int32_t alg_mode = 1;
  int32_t body_joint_set = 1;
  BodyTrackingBoneLength bone_length;
};
struct BodyTrackerCreateInfo {
  BodyTrackingAlgParam alg_param;
};
enum class BodyTrackerMode { kFullBody = 0, kStandalone };
}  // namespace ps_common
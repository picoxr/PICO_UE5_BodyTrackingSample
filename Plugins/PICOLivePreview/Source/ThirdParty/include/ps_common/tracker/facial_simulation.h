#pragma once
#include <stdint.h>

#include "ps_common/ps_exports.h"
namespace ps_common {

/**
 * reference this document
 * https://bytedance.feishu.cn/docx/NmmHdxD3noiyuOxjxREcTpOfnIg
 */

static const int32_t kMaxFaceExpressionCount = 52;
enum class FaceExpressionType {
  kEyeBlinkLeft = 0,
  kEyeLookDownLeft = 1,
  kEyeLookInLeft = 2,
  kEyeLookOutLeft = 3,
  kEyeLookUpLeft = 4,
  kEyeLookSquintLeft = 5,
  kEyeLookWideLeft = 6,

  kEyeBlinkRight = 7,
  kEyeLookDownRight = 8,
  kEyeLookInRight = 9,
  kEyeLookOutRight = 10,
  kEyeLookUpRight = 11,
  kEyeLookSquintRight = 12,
  kEyeLookWideRight = 13,

  // Mouth and Jaw
  kJawForward = 14,
  kJawLeft = 15,
  kJawRight = 16,
  kJawOpen = 17,
  kMouthClose = 18,
  kMouthFunnel = 19,
  kMouthPucker = 20,
  kMouthLeft = 21,
  kMouthRight = 22,
  kMouthSmileLeft = 23,
  kMouthSmileRight = 24,
  kMouthFrownLeft = 25,
  kMouthFrownRight = 26,
  kMouthDimpleLeft = 27,
  kMouthDimpleRight = 28,
  kMouthStretchLeft = 29,
  kMouthStretchRight = 30,
  kMouthRollLower = 31,
  kMouthRollUpper = 32,
  kMouthShrugLower = 33,
  kMouthShrugUpper = 34,
  kMouthPressLeft = 35,
  kMouthPressRight = 36,
  kMouthLowerDownLeft = 37,
  kMouthLowerDownRight = 38,
  kMouthUpperUpLeft = 39,
  kMouthUpperUpRight = 40,
  // Eyebrows, Cheeks, and Nose
  kBrownDownLeft = 41,
  kBrownDownRight = 42,
  kBrownOuterUpLeft = 44,
  kBrownOuterUpRight = 45,
  kCheekPuff = 46,
  kCheekSquintLeft = 47,
  kCheekSquintRight = 48,
  kNoseSneerLeft = 49,
  kNoseSneerRight = 50,
  // Tongue
  kTongueOut = 51,
  kCount = 52
};

static const int32_t kMaxLipExpressionCount = 20;
/**
 * pronunciation
 */
enum class LipExpressionType {
  kExpression_PP = 0,
  kExpression_CH = 1,
  kExpression_o = 2,
  kExpression_O = 3,
  kExpression_I = 4,
  kExpression_u = 5,
  kExpression_RR = 6,
  kExpression_XX = 7,
  kExpression_aa = 8,
  kExpression_i = 9,
  kExpression_FF = 10,
  kExpression_U = 11,
  kExpression_TH = 12,
  kExpression_kk = 13,
  kExpression_SS = 14,
  kExpression_e = 15,
  kExpression_DD = 16,
  kExpression_E = 17,
  kExpression_nn = 18,
  kSilence = 19,
  kCount = 20,
};

typedef uint32_t FacialSimulationMode;
static const FacialSimulationMode kFaceOnly = 0x00000001;
static const FacialSimulationMode kCombinedBs = 0x00000002;
static const FacialSimulationMode kCombinedVis = 0x00000004;
static const FacialSimulationMode kLipsOnly = 0x00000008;

struct LipExpressionData {
  float lip_expression_weight[kMaxLipExpressionCount] = {0.0f};
};

struct FacialSimulationData {
  uint64_t timestamp = 0;  // ns
  bool upper_face_data_valid = false;
  bool lower_face_data_valid = false;
  float face_expression_weight[kMaxFaceExpressionCount] = {0.0f};
  LipExpressionData lip_expression_data;
};

}  // namespace ps_common

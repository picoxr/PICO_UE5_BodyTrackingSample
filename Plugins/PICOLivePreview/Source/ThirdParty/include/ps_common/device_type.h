#pragma once
//
// Created by Administrator on 2023/5/18.
//
namespace ps_common {
enum class DeviceType {
  kUndefined = -1,
  kHead = 0,
  kHandController = 1
};
enum class SideType { kUndefined = -1, kLeft = 0, kRight = 1, kBoth = 2 };
};  // namespace ps_common

//
// Created by Admin on 2024/5/23.
//
#pragma once
#ifdef WIN32
#include "ps_base/ps_exports.h"
#include <Windows.h>
#include <setupapi.h>

#include <functional>
#include <string>
namespace ps_base{
class PS_BASE_API DeviceManager {
 public:
  DeviceManager() = default;
  ~DeviceManager() = default;
  static int EnumDevice(const std::string& class_type,
                        const std::function<int(HDEVINFO, SP_DEVINFO_DATA,
                                                DWORD)>& callback_func);
  static int EnumDevice(const std::string& class_type,
                        const std::string& hardware_id,
                        const std::function<int(HDEVINFO, SP_DEVINFO_DATA,
                                                DWORD)>& callback_func);
  static int UpdateDriver(const std::string& inf_path,
                          const std::string& hardware_id);
  static int AddDriver(const std::string& inf_path);
  static int RemoveDriver(const std::string& inf_path);
  static int InstallDevice(const std::string& inf_path,
                           const std::string& hardware_id);
  static int UninstallDevice(const std::string& class_type,
                             const std::string& hardware_id);
  static int UninstallDevice(HDEVINFO dev_info, SP_DEVINFO_DATA dev_info_data);
  static int EnableDevice(const std::string& hardware_id);

  static int GetDeviceProperty(HDEVINFO dev_info, SP_DEVINFO_DATA dev_info_data,
                               DWORD property_type, std::string& property_val);

  static std::string GetCurrentPath();
};
}
#endif

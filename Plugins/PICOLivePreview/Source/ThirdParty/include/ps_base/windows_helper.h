#pragma once
//
// Created by Alan Duan on 2024/3/28.
//
#ifdef WIN32
#include <comdef.h>
#include "ps_base/ps_exports.h"
#include "ps_base/transport/data_buffer.h"
namespace ps_base {
class PS_BASE_API WindowsHelper {
 public:
  static DataBuffer ToString(HRESULT hr);
  static DataBuffer ToString(const _com_error& error);
  static DataBuffer ToString(DWORD error_code);
  static DataBuffer GetLastErrorString();

  static DataBuffer ConvertToChar(const wchar_t * str);
};
}  // namespace ps_base
#endif

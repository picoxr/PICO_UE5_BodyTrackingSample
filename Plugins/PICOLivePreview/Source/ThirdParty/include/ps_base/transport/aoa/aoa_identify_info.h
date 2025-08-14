#pragma once
#include "ps_base/ps_exports.h"
#include "ps_base/transport/data_buffer.h"

namespace ps_base {
enum class AoaIdentifyType {
  kNone = -1,
  kManufacturerName = 0,
  kModelName = 1,
  kDescription = 2,
  kVersion = 3,
  kURI = 4,
  kSerialNumber = 5
};

class PS_BASE_API AoaIdentifyInfo {
 public:
  void Set_(AoaIdentifyType type, const char* val);
  const DataBuffer& Get_(AoaIdentifyType type) const;
  bool IsValid(AoaIdentifyType type) const;

 private:
  DataBuffer& GetMultable_(AoaIdentifyType type);

 private:
  DataBuffer none_;
  DataBuffer manufacturer_name_;
  DataBuffer model_name_;
  DataBuffer description_;
  DataBuffer version_;
  DataBuffer uri_;
  DataBuffer serial_number_;
};
}  // namespace ps_base
#pragma once
#include <vector>
namespace ps_base {
class PatternSearch {
 public:
  enum Mode { kExhaustive, kHorspool };
  PatternSearch() = default;
  ~PatternSearch() = default;

  void SetMode(Mode m) { mode_ = m; }
  bool SetPattern(uint8_t* pattern, uint8_t pattern_size);
  bool SetPattern(const std::vector<uint8_t>& pattern);
  bool Valid();
  std::vector<size_t> Search(const uint8_t* data, size_t length,
                             uint32_t skip_num = 0);

 private:
  // ExhaustiveSearch
  std::vector<size_t> ExhaustiveSearch(const uint8_t* data, size_t length,
                                       uint32_t skip_num = 0);
  //
  // HorspoolSearch
  std::vector<size_t> HorspoolSearch(const uint8_t* data, size_t length,
                                     uint32_t skip_num = 0);
  bool HorspoolGenShiftTable(const uint8_t* pattern, uint8_t pattern_size,
                             uint8_t* shift_table, size_t shift_table_size);
  int64_t HorspoolFindOnce(const uint8_t* data, size_t length,
                           const uint8_t* pattern, uint8_t pattern_size,
                           const uint8_t* shift_table);
  std::vector<size_t> HorspoolFindAll(const uint8_t* data, size_t length,
                                      const uint8_t* pattern,
                                      uint8_t pattern_size,
                                      const uint8_t* shift_table,
                                      uint32_t skip_num = 0);
  //

 private:
  Mode mode_ = kExhaustive;
  std::vector<uint8_t> pattern_;
  bool valid_ = false;

  // HorspoolSearch
  std::vector<uint8_t> shift_table_;
  //
};
}  // namespace ps_base

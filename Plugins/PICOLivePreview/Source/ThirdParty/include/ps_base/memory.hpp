
#include <memory.h>

namespace ps_base {

/// @brief memcpy in different platforms.
/// @param dest
/// @param dest_size
/// @param source
/// @param source_size ignored when non-Windows platforms.
/// @return errno_t on Windows, dest pointer on others.
inline long long ps_memcpy(void* const dest, size_t const dest_size,
                           void const* const source, size_t const source_size) {
#if defined(_WIN32)
  return memcpy_s(dest, dest_size, source, source_size);
#else
  (void)source_size;
  return (long long)memcpy(dest, source, dest_size);
#endif
};
}  // namespace ps_base
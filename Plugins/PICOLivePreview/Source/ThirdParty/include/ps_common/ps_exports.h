#pragma once

#if defined(PS_COMMON_LIB_SHARED) && \
    (defined(_WIN32) || defined(_WINDOWS) || defined(_WINDLL))
#ifdef PS_COMMON_EXPORTS
#define PS_COMMON_API __declspec(dllexport)
#else
#define PS_COMMON_API __declspec(dllimport)
#endif  // PS_COMMON_EXPORTS
#else
#define PS_COMMON_API
#endif

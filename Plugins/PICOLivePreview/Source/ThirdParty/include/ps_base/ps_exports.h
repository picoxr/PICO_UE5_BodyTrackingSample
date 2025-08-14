#pragma once

#if defined(PS_BASE_LIB_SHARED) && \
    (defined(_WIN32) || defined(_WINDOWS) || defined(_WINDLL))
#ifdef PS_BASE_EXPORTS
#define PS_BASE_API __declspec(dllexport)
#else
#define PS_BASE_API __declspec(dllimport)
#endif  // PS_BASE_EXPORTS
#else
#define PS_BASE_API
#endif

#pragma once

// Use perfetto or systrace for tracing call

//#define PS_TRACE_ON
#if defined(__ANDROID__) && defined(PS_TRACE_ON)
#include <android/trace.h>
#define PS_TRACE_BEGIN(name) ATrace_beginSection(name)
#define PS_TRACE_END() ATrace_endSection()
#else
#define PS_TRACE_BEGIN(name)
#define PS_TRACE_END()
#endif

#define PS_TRACE_NAME(name) ScopedTrace ps_tracer(name)

#if defined(PS_TRACE_ON)
#define PS_TRACE_SCOPED() PS_TRACE_NAME(__FUNCTION__)
#else
#define PS_TRACE_SCOPED()
#endif

class ScopedTrace {
 public:
  inline ScopedTrace(const char* name) { PS_TRACE_BEGIN(name); }
  inline ~ScopedTrace() { PS_TRACE_END(); }
};

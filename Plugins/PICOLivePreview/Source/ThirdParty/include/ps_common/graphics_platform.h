#pragma once
//
// Created by Administrator on 2023/6/20.
//

#ifdef _WIN32
#include <windows.h>
#include <d3d11.h>
#include <d3d12.h>
#elif defined(__ANDROID__)
#include <EGL/egl.h>
#elif defined(__MACH__)
#include <IOSurface/IOSurface.h>
#endif
#include "ps_base/transport/data_buffer.h"

namespace ps_common {
#ifdef _WIN32
struct GraphicsBindingD3D11 {
  ID3D11Device* device = nullptr;
};
struct GraphicsImageD3D11Texture2D {
  ID3D11Texture2D* texture = nullptr;
};
struct GraphicsImageD3D11SharedHandle {
  HANDLE shared_handle = INVALID_HANDLE_VALUE;
};
struct GraphicsBindingD3D12 {
  ID3D12Device* device = nullptr;
  ID3D12CommandQueue* queue = nullptr;
};
struct GraphicsImageD3D12Resource {
  ID3D12Resource** texture = nullptr;
};
#elif defined(__ANDROID__)
struct GraphicsBindingOpenGLESAndroid {
  EGLDisplay display = EGL_NO_DISPLAY;
  EGLSurface surface = EGL_NO_SURFACE;
  EGLContext context = EGL_NO_CONTEXT;
  EGLConfig config = nullptr;
};
struct GraphicsImageOpenGLESAndroidTexture {
  uint32_t gl_texture = 0;
  uint32_t width = 0;
  uint32_t height = 0;
};
#elif defined(__MACH__)
struct GraphicsBindingMetal {
  uint64_t gpu_registry_id = 0;
};
struct GraphicsBindingCoreVideo {};
struct GraphicsImageCoreVideoPixelBuffer {
  IOSurfaceID io_surface_id = 0;
};
#endif
struct GraphicsImageRawMemory {
  ps_base::DataBuffer image_data;
};
}  // namespace ps_common

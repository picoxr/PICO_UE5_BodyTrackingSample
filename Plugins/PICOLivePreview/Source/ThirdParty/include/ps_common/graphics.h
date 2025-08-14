#pragma once
//
// Created by Administrator on 2023/6/20.
//
#include "ps_common/graphics_platform.h"
namespace ps_common {
enum class PixelFormat{
  kR8G8B8A8_Uniform = 0,
  kR8G8B8A8_sRGB = 1
};
enum class GraphicsApiType {
  kUndefined = -1,
  kRawMemory = 0,
  kD3D11 = 10001,
  kD3D12 = 10002,
  kOpenGLESAndroid = 20001,
  kCoreVideo = 30001,
  kMetal = 30002,
};
struct GraphicsBinding {
  GraphicsApiType api_type = GraphicsApiType::kUndefined;
#ifdef _WIN32
  GraphicsBindingD3D11 d3d11_binding;
  GraphicsBindingD3D12 d3d12_binding;
#elif defined(__ANDROID__)
  GraphicsBindingOpenGLESAndroid opengles_android_binding;
#elif defined(__MACH__)
  GraphicsBindingCoreVideo core_video_binding;
#endif
};
enum class GraphicsImageType {
  kUndefined = -1,
  kRawMemory = 0,
  kD3D11Texture2D = 10001,
  kD3D11SharedHandle = 10002,
  kD3D12Resource = 10003,
  kOpenGLESTexture = 20001,
  kVulkanMemoryFd = 20002,
  kCoreVideoPixelBuffer = 30001,
};
struct GraphicsImage {
  GraphicsImageType image_type = GraphicsImageType::kUndefined;
#ifdef _WIN32
  GraphicsImageD3D11Texture2D d3d11_image;
  GraphicsImageD3D11SharedHandle d3d11_shared_image;
  GraphicsImageD3D12Resource d3d12_image;
#elif defined(__ANDROID__)
  GraphicsImageOpenGLESAndroidTexture opengles_android_image;
#elif defined(__MACH__)
  GraphicsImageCoreVideoPixelBuffer core_video_pixel_buffer;
#endif
};
}  // namespace ps_common
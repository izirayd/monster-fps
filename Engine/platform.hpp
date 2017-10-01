#pragma once

#if defined(_WIN32) || defined(_WIN64)
 #define PLATFORM_WINDOWS
#endif

#if defined(__GNUC__)
  #define PLATFORM_LINUX
#endif 

#if defined(__ANDROID__)
  #define PLATFORM_ANDROID
#endif
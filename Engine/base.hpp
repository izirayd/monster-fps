#pragma once

#include "platform.hpp"

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <fcntl.h>
#include <cstring>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <limits.h>
#include <stdarg.h>
#include <vector>
#include <typeinfo>
#include <chrono>
#include <sstream>

#if defined(PLATFORM_WINDOWS) 
#include <xstring>
#include <io.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <Shlwapi.h>
#endif

#if defined(PLATFORM_LINUX) 
#include <unistd.h>
#endif

#if defined(PLATFORM_WINDOWS)
#define VSNPRINTF		 _vsnprintf
#define VSNWPRINTF		 _vsnwprintf
#define OriSpintfUnicode  wsvnprintf
#endif

#if defined(PLATFORM_LINUX)
#define VSNPRINTF		 vsnprintf
#define VSNWPRINTF		 vswprintf
#define OriSpintfUnicode swprintf
#endif

#pragma warning(disable : 4996)

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "Shlwapi.lib")
/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

//	:/Title	Project-base entry-point
//	:/Text	This is that entry point of the project_base library. This is the only file you
//			will have to include to use this library. All functionality will be available 
//			by including this file.
//


//	:/Title	Source-code tags
//	:/Text	Throughout the source code tags prefixed with @ can be found marking the code below it.
//			- @Hardcoded: hardcoded buffer sizes. Should be fixed.
//			- @Unused: Unused function that should be removed.
//			- @Speed: Slow code that should be rewritten eventually.

#ifndef INCLUDE_PROJECT_BASE
#define INCLUDE_PROJECT_BASE

#define PROJECT_BASE_NAME "Project-base"
#define PROJECT_BASE_VERSION "2.0.0"

#ifndef TARGET_FRAMERATE
#define TARGET_FRAMERATE (1000/30.0)
#endif

#if defined(_MSC_VER)
#define COMPILER_MSVC
#endif

#if defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)
#define COMPILER_GCC
#endif

#ifdef _WIN32
#define OS_WIN
#include <winsock2.h>
#endif
#ifdef __linux__
#define OS_LINUX
#endif
#ifdef __APPLE__
#define OS_OSX
#error platform not supported
#endif

#include "stdint.h"
#include "string.h"

#define s8 int8_t
#define s16 int16_t
#define s32 int32_t
#define s64 int64_t

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define float32 float
#define float64 double

#define f32 float
#define f64 double

#ifdef OS_LINUX
#define bool uint8_t
#endif
#ifdef OS_WIN
#define bool _Bool
#endif

#define true 1
#define false 0

#include "logging.h"
#include "thread.h"
#include "array.h"
#include "memory.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "external/stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"

#include "lib_loader.h"
#include "external/utf8.h"
#include "input.h"
#include "assets.h"
#include "audio.h"
#include "memory_bucket.h"
#include "render.h"
#include "camera.h"
#include "platform.h"
#include "popup_window.h"
#include "ui.h"
#include "string_utils.h"
#include "settings_config.h"
#include "localization.h"
#include "networking.h"

//#ifdef LIBPROJECTBASE_IMPLEMENTATION

#ifdef OS_LINUX
#include "linux/lib_loader.c"
#include "linux/thread.c"
#include "linux/platform.c"
#include "linux/networking.c"
#endif

#ifdef OS_WIN
#include "windows/lib_loader.c"
#include "windows/thread.c"
#include "windows/platform.c"
#include "windows/networking.c"
#endif

#include "platform_shared.c"

#include "renderers/render_gl.c"
#include "renderers/render_cpu.c"
#include "render.c"
#include "input.c"
#include "array.c"
#include "assets.c"
#include "audio.c"
#include "camera.c"
#include "ui.c"
#include "popup_window.c"
#include "string_utils.c"
#include "settings_config.c"
#include "localization.c"
#include "memory_bucket.c"

//#endif
#endif
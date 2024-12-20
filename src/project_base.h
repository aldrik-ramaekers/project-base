/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_PROJECT_BASE
#define INCLUDE_PROJECT_BASE

#define PROJECT_BASE_VERSION "3"

#ifdef _WIN32
#define OS_WIN
#include <windows.h>
#include <time.h>
#endif
#ifdef __linux__
#define OS_LINUX
#include <sys/times.h>
#include <sys/vtimes.h>
#endif
#ifdef __APPLE__
#define OS_OSX
#error platform not supported
#endif

#include "stdint.h"
#include "string.h"
#include "assert.h"

#include <GL/gl.h>
#ifdef OS_LINUX
#include <GL/glx.h>
#endif
#include <GL/glu.h>
#include <GL/glext.h>

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

#include "thread.h"
#include "array.h"
#include "memory.h"
#include "external/cJSON.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "external/stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"

#include "external/utf8.h"
#include "input.h"
#include "timer.h"
#include "assets.h"
#include "memory_bucket.h"
#include "platform.h"
#include "render.h"
#include "camera.h"
#include "ui.h"
#include "notification.h"
#include "string_utils.h"
#include "settings_config.h"
#include "localization.h"

#include "platform_shared.c"

#ifdef OS_LINUX
#include "linux/thread.c"
#include "linux/platform.c"
#endif

#ifdef OS_WIN
#include "windows/thread.c"
#include "windows/platform.c"
#endif

#include "render.c"
#include "input.c"
#include "timer.c"
#include "array.c"
#include "assets.c"
#include "camera.c"
#include "ui.c"
#include "notification.c"
#include "string_utils.c"
#include "settings_config.c"
#include "localization.c"
#include "memory_bucket.c"
#include "external/cJSON.c"

#endif
/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

/*
:docs:
<h1>Project-base</h1>
This is that entry point of the project_base library. This is the only file you
will have to include to use this library. All files will be imported by including
this file.

<h2>Introduction</h2>

*/

/*

- move convert_crlf_to_lf to string utils
- rename get_filters to something more obvious and move to respective file
- generate docs
- prefix private functions with _ like platform_destroy_shared
- fix indentation of definitions so name is on same column
- get_active_directory and get_active_directory prefix with platform_
- move string_to_[type] to string utils
- return code for platform_delete_file
- move filter_matches to string utils and rename to string_split
- make custom popup windows so we can get rid of zenity and windows deps
- stop linking to libs and use LoadLibrary
- let the library handle asset destroying
- make settings pages use settings_config directly
- get rid of text-search code in platform.h

*/

#ifndef INCLUDE_PROJECT_BASE
#define INCLUDE_PROJECT_BASE

#pragma GCC diagnostic ignored "-Wunused-result"

#define PROJECT_BASE_VERSION "2.0.0"

#ifndef TARGET_FRAMERATE
#define TARGET_FRAMERATE (1000/24.0)
#endif

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

#include "resources.h"
#include "thread.h"
#include "array.h"
#include "memory.h"
#include "external/cJSON.h"

#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "external/stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb_truetype.h"
#pragma GCC diagnostic warning "-Wmaybe-uninitialized"

#include "external/utf8.h"
#include "input.h"
#include "timer.h"
#include "assets.h"
#include "memory_bucket.h"
#include "camera.h"
#include "platform.h"
#include "render.h"
#include "ui.h"
#include "notification.h"
#include "string_utils.h"
#include "settings_config.h"
#include "localization.h"

#ifdef OS_LINUX
#include "linux/thread.c"
#include "linux/platform.c"
#endif

#ifdef OS_WIN
#include "windows/thread.c"
#include "windows/platform.c"
#endif

#include "platform_shared.c"

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
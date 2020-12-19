/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

/*
    :/Title Project-base entry-point
    :/Text  This is that entry point of the project_base library. This is the only file you
            will have to include to use this library. All functionality will be available 
            by including this file.
*/

/*
    :/Title TODO's
    :/Text  Project-base
            - make sure a valid render config is found on linux
            - replace all int with s32
            - remove all unused functions @Unused
            - prefix private functions with _ like platform_destroy_shared
            - make custom popup windows so we can get rid of zenity and windows deps
            - stop linking to libs and use dlopen on linux
            - add messages to all asserts

    :/Text  Text-search
            - make settings pages use settings_config directly
            - get rid of text-search code in platform.h
*/

#ifndef INCLUDE_PROJECT_BASE
#define INCLUDE_PROJECT_BASE

#pragma GCC push_options

#if defined(MODE_DEBUG)
#pragma GCC optimize("Og")
#else
#pragma GCC optimize("O3")
#define NDEBUG
#endif

#pragma GCC diagnostic ignored "-Wunused-result"

#define PROJECT_BASE_NAME "Project-base"
#define PROJECT_BASE_VERSION "2.0.0"

#ifndef TARGET_FRAMERATE
#define TARGET_FRAMERATE (1000/24.0)
#endif

#ifdef _WIN32
#define OS_WIN
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

#include "lib_loader.h"
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
#include "linux/lib_loader.c"
#include "linux/thread.c"
#include "linux/platform.c"
#endif

#ifdef OS_WIN
#include "windows/lib_loader.c"
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

#pragma GCC pop_options

#endif
/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_LOGGING
#define INCLUDE_LOGGING

#if defined(MODE_DEBUG)

#include <string.h>
#define __FILENAME__ (strrchr("/" __FILE__, '/') + 1)

#ifdef OS_LINUX
#define log_assert(_cond, _message) if (!(_cond)) { printf("ERROR: %s, Assertion failed at %s, line %d. In %s\n", _message, __FILENAME__, __LINE__, __func__); exit(0); }
#define log_info(_message) { printf("INFO: %s, at %s, line %d. In %s\n", _message,  __FILENAME__, __LINE__, __func__); }
#endif

#ifdef OS_WIN
#define log_assert(_cond, _message) if (!(_cond)) \
    {   char __buf[500]; \
        sprintf(__buf, "%s, Assertion failed at %s, line %d. In %s\n", _message, __FILENAME__, __LINE__, __func__); \
        printf(__buf); \
        MessageBox(0, __buf, "Error", MB_OK); exit(0); }
#define log_info(_message) { char _____buf[4000]; sprintf(_____buf, "INFO: %s, at %s, line %d. In %s\n", _message,  __FILENAME__, __LINE__, __func__); printf(_____buf); OutputDebugStringA(_____buf); }
#endif

#else

#define log_assert(_cond, _message) do{}while(0)
#define log_info( _message) do{}while(0)

#endif

#define log_assert_m(_cond) log_assert(_cond, "")

#endif
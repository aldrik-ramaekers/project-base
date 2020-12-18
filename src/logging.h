/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_LOGGING
#define INCLUDE_LOGGING

#if defined(MODE_DEBUG)

#ifdef OS_LINUX
#define log_assert(_cond, _message) if (!(_cond)) { printf("ERROR: %s, Assertion failed at %s, line %d.\n", _message, __FILE__, __LINE__); exit(0); }
#define log_info(_message) { printf("INFO: %s, at %s, line %d.\n", _message,  __FILE__, __LINE__, ); }
#endif

#ifdef OS_WIN
#define log_assert(_cond, _message) if (!(_cond)) \
{ char __buf[500]; \
sprintf(__buf, "ERROR: %s, Assertion failed at %s, line %d.\n", _message, __FILE__, __LINE__); \
printf(__buf); \
MessageBox(0, __buf, "Error", MB_OK); exit(0); }

#define log_info(_message) { printf("INFO: %s, at %s, line %d.\n", _message,  __FILE__, __LINE__, ); }
#endif

#else

#define log_assert(_cond, _message) do{}while(0)
#define log_info( _message) do{}while(0)

#endif

#define log_assert_m(_cond) log_assert(_cond, "")

#endif
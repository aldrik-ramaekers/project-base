/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_LIB_LOADER
#define INCLUDE_LIB_LOADER

#include <GL/gl.h>
#ifdef OS_WIN
#include <GL/wglext.h>
#endif
#ifdef OS_LINUX
#include <GL/glx.h>
#endif
#include <GL/glu.h>
#include <GL/glext.h>

//	:/Info	Load all required dynamic libraries. Will exit the application when a library or function is missing.
void _lib_loader_init();

#endif
/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_LIB_LOADER
#define INCLUDE_LIB_LOADER

//	:/Title	Library loader
//	:/Text	Contains functions to load the necessary libraries.

#include "external/GL/gl.h"
#ifdef OS_WIN
#include "external/GL/wglext.h"
#endif
#ifdef OS_LINUX
#include "external/GL/glx.h"
#endif
#include "external/GL/glu.h"
#include "external/GL/glext.h"

//	:/Info	Load all required dynamic libraries. Will exit the application when a library or function is missing.
void _lib_loader_init();

#endif
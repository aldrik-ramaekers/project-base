/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_MEMORY
#define INCLUDE_MEMORY

#ifdef MODE_DEVELOPER
static s32 __total_allocated = 0;
static s32 __total_reallocated = 0;

#define mem_alloc(size) malloc(size); __total_allocated+=size
#define mem_free(p) free(p)
#define mem_realloc(p, size) realloc(p, size); __total_reallocated+=size
#define memory_print_leaks() {}

#else

#define mem_alloc(size) malloc(size)
#define mem_free(p) free(p)
#define mem_realloc(p, size) realloc(p, size)
#define memory_print_leaks() {}

#endif

#define STBI_MALLOC(sz) mem_alloc(sz)
#define STBI_REALLOC(p, newsz) mem_realloc(p, newsz)
#define STBI_FREE(p) mem_free(p)

#endif
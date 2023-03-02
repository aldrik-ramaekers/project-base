/*
 *  BSD 2-Clause “Simplified” License
 *  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
 *  All rights reserved.
 */

#ifndef INCLUDE_MEMORY
#define INCLUDE_MEMORY

#if 0
#define mem_alloc(size) malloc(size);
#define mem_alloc_p(__caller, __size) malloc(__size)
#define mem_free(p) free(p)
#define mem_realloc(p, size) realloc(p, size)
#define memory_print_leaks() \
	{                        \
	}
#else
#include <stdio.h>
typedef struct t_mem_entry
{
	void *ptr;
	u64 len;
	const char* caller;
} mem_entry;
array _mem_registry = {0, 0, 0, 0, 0};
mutex _mem_mut;

#ifdef __GNUC__

void *_registered_alloc(const char* calling_func, u64 size)
#else
__declspec(allocator) void *_registered_alloc(u64 size)
#endif
{
	if (_mem_registry.data == 0)
	{
		_mem_mut = mutex_create();
		array new_array;
		new_array.length = 0;
		new_array.reserved_length = 0;
		new_array.entry_size = sizeof(mem_entry);
		new_array.data = malloc(new_array.entry_size);
		new_array.reserve_jump = 1;
		new_array.mutex = mutex_create_recursive();

		_mem_registry = new_array;
		array_reserve(&_mem_registry, 1000);
	}
	mutex_lock(&_mem_mut);
	void *addr = malloc(size);
	mem_entry new_entry;
	new_entry.ptr = addr;
	new_entry.len = size;
	new_entry.caller = calling_func;
	array_push(&_mem_registry, (uint8_t *)&new_entry);
	mutex_unlock(&_mem_mut);
	return addr;
}

void _registered_free(void *addr)
{
	mutex_lock(&_mem_mut);
	for (int i = 0; i < _mem_registry.length; i++)
	{
		mem_entry *entry = array_at(&_mem_registry, i);
		if (entry->ptr == addr)
		{
			array_remove_at(&_mem_registry, i);
			free(addr);
			mutex_unlock(&_mem_mut);
			return;
		}
	}
	mutex_unlock(&_mem_mut);
}

void* _registered_realloc(void* addr, u64 size)
{
	mutex_lock(&_mem_mut);
	void* res = realloc(addr, size);
	for (s32 i = 0; i < _mem_registry.length; i++)
	{
		mem_entry *entry = array_at(&_mem_registry, i);
		if (entry->ptr == addr) {
			entry->ptr = res;
			mutex_unlock(&_mem_mut);
			return res;
		}
	}
	mutex_unlock(&_mem_mut);
	return res;
}

void _registered_print_leaks()
{
	mutex_lock(&_mem_mut);
	printf("Leaks:\n");
	u64 totalLeaked = 0;
	for (s32 i = 0; i < _mem_registry.length; i++)
	{
		mem_entry *entry = array_at(&_mem_registry, i);
		printf("%s -> %llu\n", entry->caller, entry->len);
		totalLeaked += entry->len;
	}
	printf("%llu\n", totalLeaked);
	mutex_unlock(&_mem_mut);
}

#define mem_alloc(size) _registered_alloc(__func__, size);
#define mem_alloc_p(__caller, __size) _registered_alloc(__caller, __size)
#define mem_free(p) _registered_free(p)
#define mem_realloc(p, size) _registered_realloc(p, size)
#define memory_print_leaks() _registered_print_leaks();
#endif

#define STBI_MALLOC(sz) mem_alloc(sz)
#define STBI_REALLOC(p, newsz) mem_realloc(p, newsz)
#define STBI_FREE(p) mem_free(p)

#endif
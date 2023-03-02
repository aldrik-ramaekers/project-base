/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_ARRAY
#define INCLUDE_ARRAY

typedef struct t_array
{
	u32		length;
	u32		reserved_length;
	u64		entry_size;
	u32		reserve_jump;
	u8*	data;
	mutex	mutex;
} array;

#define array_push(__arr, __data) _array_push(__func__, __arr, __data)
#define array_reserve(__arr, __count) _array_reserve(__func__, __arr, __count)

array 	array_create(u64 entry_size);
bool 	array_exists(array *arr);
int 	_array_push(const char* caller, array *arr, u8 *data);
int 	array_push_size(array *arr, u8 *data, s32 entry_size);
void 	array_remove_at(array *arr, u32 at);
void 	array_remove(array *arr, u8 *ptr);
void 	array_remove_by(array *arr, u8 *data);
void* 	array_at(array *arr, u32 at);
void 	array_destroy(array *arr);
void 	array_swap(array *arr, u32 swap1, u32 swap2);
void 	_array_reserve(const char* caller, array *arr, u32 reserve_count);
array 	array_copy(array *arr);
void 	array_clear(array *arr);
int 	array_push_at(array *array, u8 *data, u32 index);

#endif
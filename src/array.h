/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_ARRAY
#define INCLUDE_ARRAY

//	:/Title	Array
//	:/Text	A simple thread-safe array implementation is provided with the Project-base library.
//			This implementation provides functionality for adding, deleting and reordering data.

typedef struct t_array
{
	u32		length;
	u32		reserved_length;
	u64		entry_size;
	u32		reserve_jump;
	u8*	data;
	mutex	mutex;
} array;

//	:/Info 	Create an empty array for items of size `entry_size`. No allocations will be made.
//	:/Ret	An empty array.
array 	array_create(u64 entry_size);

//	:/Info	Validate the given array is correctly instantiated. Will return false after calling `array_destroy` on it.
//	:/Ret	true when valid, false when invalid.
bool 	array_exists(array *arr);

//	:/Info	Add an item to the end of the array.
//	:/Ret	The index of the new item within the array.
int 	array_push(array *arr, u8 *data);

//	:/Info	Add an item of given size `entry_size` to the end of the array. Remaining item space is filled with 0.
//			Writing an item bigger than the entry size specified in `array_create` is not allowed.
//	:/Ret	The index of the new item within the array.
int 	array_push_size(array *arr, u8 *data, s32 entry_size);

//	:/Info	Remove the item at the given index.
void 	array_remove_at(array *arr, u32 at);

//	:/Info	Remove an item by the given address returned by `array_at`.
void 	array_remove(array *arr, u8 *ptr);

//	:/Info	Remove an item with the given data.
void 	array_remove_by(array *arr, u8 *data);

//	:/Info	Retrieve the item at the given index.
//	:/Ret	The address within the array pointing to the data.
void* 	array_at(array *arr, u32 at);

//	:/Info	Destroy the given array. This array should not be used after calling this function.
void 	array_destroy(array *arr);

//	:/Info	Swap 2 items within the array at the given indexes.
void 	array_swap(array *arr, u32 swap1, u32 swap2);

//	:/Info	Reserve `reserve_count` amount of item slots. 
//			`reserve_count` is the total amount of available slots, it is not appended.
void 	array_reserve(array *arr, u32 reserve_count);

//	:/Info	Copy the given array and items within the array.
//	:/Ret	A copy of the given array.
array 	array_copy(array *arr);

//	:/Info	Clear tha array. Amount of reserve spots remain.
void 	array_clear(array *arr);

int 	array_push_at(array *array, u8 *data, u32 index);

#endif
/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#ifndef INCLUDE_ARRAY
#define INCLUDE_ARRAY

#define ASSERT(e_) {if(!(e_)){*(int*)0=0;}}

//	:/Title Array
//	:/Text	A simple thread-safe array implementation is provided with the Project-base library.
//			This implementation provides basic functionality for adding, deleting and changing data.

typedef struct t_array
{
	u32 	length;
	u32 	reserved_length;
	u64 	entry_size;
	u32 	reserve_jump;
	void*	data;
	mutex 	mutex;
} array;

// :/Info 	Create an empty array for items of size `entry_size`. No allocations will be made.
// :/Return An empty array.
array 	array_create(u64 entry_size);

// :/Info	Validate the given array is correctly instantiated. Will return false after calling `array_destroy` on it.
// :/Return true when valid, false when invalid.
bool 	array_exists(array *array);

// :/Info	Add an item to the end of the array.
// :/Return The index of the new item within the array.
int 	array_push(array *array, void *data);

// :/Info	Add an item of given size `entry_size` to the end of the array. Remaining item space is filled with 0.
//			Writing an item bigger than the entry size specified in `array_create` is not allowed.
// :/Return	The index of the new item within the array.
int 	array_push_size(array *array, void *data, s32 entry_size);

// :/Info	Remove the item at the given index.
void 	array_remove_at(array *array, u32 at);

// :/Info	Remove an item by the given address returned by `array_at`.
void 	array_remove(array *array, void *ptr);

// :/Info	Remove an item by the given data.
void 	array_remove_by(array *array, void *data);

// :/Info	Retrieve the item at the given index.
// :Return	The address within the array pointing to the data.
void* 	array_at(array *array, u32 at);

// :/Info	Destroy the given array. This array should not be used after calling this function.
void 	array_destroy(array *array);

// :/Info	Swap 2 items within the array at the given indexes.
void 	array_swap(array *array, u32 swap1, u32 swap2);

// :/Info	Reserve `reserve_count` amount of item slots. 
//			`reserve_count` is the total amount of available slots, it is not appended.
void 	array_reserve(array *array, u32 reserve_count);

// :/Info	Copy the given array and items within the array.
// :Return	A copy of the given array.
array 	array_copy(array *array);

#endif
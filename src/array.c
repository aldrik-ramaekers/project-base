/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

array array_create(u64 entry_size)
{
	array new_array;
	new_array.length = 0;
	new_array.reserved_length = 0;
	new_array.entry_size = entry_size;
	new_array.data = 0;
	new_array.reserve_jump = 1;
	new_array.mutex = mutex_create_recursive();
	
	return new_array;
}

bool array_exists(array *array)
{
	return array->entry_size;
}

int array_push_at(array *array, u8 *data, u32 index)
{
	log_assert(array, "Array cannot be null");
	log_assert(data, "Data to insert cannot be null");
	log_assert(array->reserve_jump >= 1, "Array reserve_jump cannot be less than 1. Array is invalid.");
	
	mutex_lock(&array->mutex);
	log_assert(index >= 0 && index < array->length, "Index is out of bounds");
	//array->length++;
	
	if (array->reserved_length < array->length)
	{
		array->reserved_length += array->reserve_jump;
		array->data = mem_realloc(array->data, (array->reserved_length*array->entry_size));
	}
	
	//memcpy(data + ((index+1) * array->entry_size), data + (index * array->entry_size), array->entry_size*(array->length-index));
	memcpy(array->data + (index * array->entry_size), data, array->entry_size);
	
	s32 result = array->length -1;
	mutex_unlock(&array->mutex);
	return result;
}

int array_push(array *array, u8 *data)
{
	log_assert(array, "Array cannot be null");
	log_assert(data, "Data to insert cannot be null");
	log_assert(array->reserve_jump >= 1, "Array reserve_jump cannot be less than 1. Array is invalid.");
	
	mutex_lock(&array->mutex);
	array->length++;
	
	if (!array->data)
	{
		array->data = mem_alloc(array->entry_size * array->reserve_jump);
		array->reserved_length = array->reserve_jump;
	}
	
	if (array->reserved_length < array->length)
	{
		array->reserved_length += array->reserve_jump;
		array->data = mem_realloc(array->data, (array->reserved_length*array->entry_size));
	}
	
	memcpy(array->data + ((array->length-1) * array->entry_size),
		   data, array->entry_size);
	
	s32 result = array->length -1;
	mutex_unlock(&array->mutex);
	return result;
}

int array_push_size(array *array, u8 *data, s32 entry_size)
{
	log_assert(array, "Array cannot be null");
	log_assert(data, "Data to insert cannot be null");
	log_assert(entry_size <= array->entry_size, "Size of data to insert cannot be larger than the entry size passed to array_create()");
	log_assert(array->reserve_jump >= 1, "Array reserve_jump cannot be less than 1. Array is invalid.");
	
	mutex_lock(&array->mutex);
	array->length++;
	
	if (!array->data)
	{
		array->data = mem_alloc(array->entry_size * array->reserve_jump);
		array->reserved_length = array->reserve_jump;
	}
	
	if (array->reserved_length < array->length)
	{
		array->reserved_length += array->reserve_jump;
		array->data = mem_realloc(array->data, (array->reserved_length*array->entry_size));
	}
	
	memcpy(array->data + ((array->length-1) * array->entry_size),
		   data, entry_size);
	
	// fill remaining space with 0
	if (array->entry_size > entry_size)
	{
		s32 remaining = array->entry_size - entry_size;
		memset(array->data + ((array->length-1) * array->entry_size) + entry_size,
			   0, remaining);
	}
	
	s32 result = array->length -1;
	mutex_unlock(&array->mutex);
	return result;
}

void array_reserve(array *array, u32 reserve_count)
{
	log_assert(array, "Array cannot be null");
	
	mutex_lock(&array->mutex);
	u32 reserved_count = array->reserved_length - array->length;
	reserve_count -= reserved_count;
	
	if (reserve_count > 0)
	{
		array->reserved_length += reserve_count;
		
		if (array->data)
		{
			array->data = mem_realloc(array->data, (array->reserved_length*array->entry_size));
		}
		else
		{
			array->data = mem_alloc(array->reserved_length*array->entry_size);
		}
	}
	mutex_unlock(&array->mutex);
}

void array_remove_at(array *array, u32 at)
{
	log_assert(array, "Array cannot be null");
	log_assert(at >= 0, "Index to remove cannot be smaller than 0.");
	log_assert(at < array->length, "Index to remove is out of bounds.");
	
	mutex_lock(&array->mutex);
	if (array->length > 1)
	{
		int offset = at * array->entry_size;
		int size = (array->length - at - 1) * array->entry_size;
		memcpy(array->data + offset,
			   array->data + offset + array->entry_size,
			   size);
		
		//array->data = realloc(array->data, array->length * array->entry_size);
	}
	
	array->length--;
	mutex_unlock(&array->mutex);
}

void array_remove(array *array, u8 *ptr)
{
	mutex_lock(&array->mutex);
	int offset = ptr - array->data;
	int at = offset / array->entry_size;
	array_remove_at(array, at);
	mutex_unlock(&array->mutex);
}

void array_remove_by(array *array, u8 *data)
{
	log_assert(array, "Array cannot be null");
	
	mutex_lock(&array->mutex);
	for (int i = 0; i < array->length; i++)
	{
		u8 *d = array_at(array, i);
		if (memcmp(d, data, array->entry_size) == 0)
		{
			array_remove_at(array, i);
			return;
		}
	}
	mutex_unlock(&array->mutex);
}

void *array_at(array *array, u32 at)
{
	mutex_lock(&array->mutex);
	log_assert(array, "Array cannot be null");
	log_assert(at >= 0, "Index to return cannot be smaller than 0.");
	log_assert(at < array->length, "Index to return is out of bounds.");
	
	u8 *result =  array->data + (at * array->entry_size);
	mutex_unlock(&array->mutex);
	return result;
}

void array_destroy(array *array)
{
	log_assert(array, "Array cannot be null");
	mem_free(array->data);
	mutex_destroy(&array->mutex);
	array->entry_size = 0;
}

void array_swap(array *array, u32 swap1, u32 swap2)
{
	log_assert(array, "Array cannot be null");
	log_assert(swap1 >= 0, "Source index to swap cannot be smaller than 0.");
	log_assert(swap1 < array->length, "Source index to swap is out of bounds.");
	log_assert(swap2 >= 0, "Destination index to swap cannot be smaller than 0.");
	log_assert(swap2 < array->length, "Destination index to swap is out of bounds.");
	if (swap1 == swap2) return;
	
	u8 *swap1_at = array_at(array, swap1);
	u8 *swap2_at = array_at(array, swap2);
	
	mutex_lock(&array->mutex);
	char* swap1_buffer = mem_alloc(array->entry_size);
	memcpy(swap1_buffer, swap1_at, array->entry_size);
	memcpy(swap1_at, swap2_at, array->entry_size);
	memcpy(swap2_at, swap1_buffer, array->entry_size);
	mutex_unlock(&array->mutex);
	mem_free(swap1_buffer);
}

array array_copy(array *arr)
{
	array new_array;
	new_array.length = arr->length;
	new_array.reserved_length = arr->reserved_length;
	new_array.entry_size = arr->entry_size;
	new_array.reserve_jump = arr->reserve_jump;
	new_array.data = mem_alloc(new_array.entry_size*new_array.reserved_length);
	new_array.mutex = mutex_create();
	
	mutex_lock(&arr->mutex);
	memcpy(new_array.data, arr->data, new_array.entry_size*new_array.reserved_length);
	mutex_unlock(&arr->mutex);
	return new_array;
}

void array_clear(array *arr)
{
	mutex_lock(&arr->mutex);
	arr->length = 0;
	mutex_unlock(&arr->mutex);
}
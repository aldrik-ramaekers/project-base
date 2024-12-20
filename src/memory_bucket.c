/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

inline memory_bucket memory_bucket_init(s32 bucket_size)
{
	assert(bucket_size >= MAX_INPUT_LENGTH);
	
	memory_bucket collection;
	collection.bucket_mutex = mutex_create();
	collection.buckets = array_create(sizeof(memory_bucket_entry));
	
	memory_bucket_entry bucket;
	bucket.data = mem_alloc(bucket_size);
	bucket.length = bucket_size;
	bucket.cursor = 0;
	array_push(&collection.buckets, &bucket);
	return collection;
}

void* memory_bucket_reserve(memory_bucket *bucket, s32 reserve_length)
{
	mutex_lock(&bucket->bucket_mutex);
	memory_bucket_entry *bucket_entry = 0;
	for (s32 i = 0; i < bucket->buckets.length; i++)
	{
		bucket_entry = array_at(&bucket->buckets, i);
		
		if (bucket_entry->length - bucket_entry->cursor < reserve_length) continue;
		
		void *space = bucket_entry->data+bucket_entry->cursor;
		bucket_entry->cursor += reserve_length;
		mutex_unlock(&bucket->bucket_mutex);
		
		return space;
	}
	
	// failed to find suitable space, allocate new bucket
	memory_bucket_entry new_bucket;
	new_bucket.data = mem_alloc(bucket_entry->length);
	new_bucket.length = bucket_entry->length;
	new_bucket.cursor = 0;
	array_push(&bucket->buckets, &new_bucket);
	mutex_unlock(&bucket->bucket_mutex);
	
	return new_bucket.data;
}

inline void memory_bucket_reset(memory_bucket *bucket)
{
	mutex_lock(&bucket->bucket_mutex);
	for (s32 i = 0; i < bucket->buckets.length; i++)
	{
		memory_bucket_entry *bucket_entry = array_at(&bucket->buckets, i);
		bucket_entry->cursor = 0;
	}
	mutex_unlock(&bucket->bucket_mutex);
}

inline void memory_bucket_destroy(memory_bucket *bucket)
{
	mutex_lock(&bucket->bucket_mutex);
	for (s32 i = 0; i < bucket->buckets.length; i++)
	{
		memory_bucket_entry *bucket_entry = array_at(&bucket->buckets, i);
		mem_free(bucket_entry->data);
	}
	array_destroy(&bucket->buckets);
	mutex_unlock(&bucket->bucket_mutex);
	
	mutex_destroy(&bucket->bucket_mutex);
}
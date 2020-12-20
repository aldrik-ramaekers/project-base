/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

// stop gcc from reporting implicit declaration warning..
extern long int syscall (long int __sysno, ...);
//extern int pthread_tryjoin_np(IMP_pthread_t thread, void **retval);

thread thread_start(void *(*start_routine) (void *), void *arg)
{
	thread result;
	result.valid = false;
	
	pthread_attr_t attr;
	int attr_init_result = IMP_pthread_attr_init(&attr);
	if (attr_init_result)
		return result;
	
	int start_thread_result = IMP_pthread_create(&result.thread, &attr, start_routine, arg);
	if (start_thread_result)
	{
		IMP_pthread_attr_destroy(&attr);
		return result;
	}
	
	result.valid = true;
	IMP_pthread_attr_destroy(&attr);
	
	return result;
}

inline void thread_detach(thread *thread)
{
	if (thread->valid)
	{
		IMP_pthread_detach(thread->thread);
	}
}

inline void thread_join(thread *thread)
{
	if (thread->valid)
	{
		void *retval;
		IMP_pthread_join(thread->thread, &retval);
	}
}

inline bool thread_tryjoin(thread *thread)
{
	if (thread->valid)
	{
		void *retval;
		bool thread_joined = !IMP_pthread_tryjoin_np(thread->thread, &retval);
		return thread_joined;
	}
	return false;
}

inline void thread_exit()
{
	IMP_pthread_exit(0);
}

inline void thread_stop(thread *thread)
{
	if (thread->valid)
	{
		IMP_pthread_cancel(thread->thread);
	}
}

mutex mutex_create()
{
	mutex result;
	
	pthread_mutexattr_t attr;
	IMP_pthread_mutexattr_init(&attr);
	IMP_pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	
	IMP_pthread_mutex_init(&result.mutex, &attr);
	
	IMP_pthread_mutexattr_destroy(&attr);
	
	return result;
}

mutex mutex_create_recursive()
{
	mutex result;
	
	pthread_mutexattr_t attr;
	IMP_pthread_mutexattr_init(&attr);
	IMP_pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	
	IMP_pthread_mutex_init(&result.mutex, &attr);
	
	IMP_pthread_mutexattr_destroy(&attr);
	
	return result;
}

inline void mutex_lock(mutex *mutex)
{
	IMP_pthread_mutex_lock(&mutex->mutex);
}

inline bool mutex_trylock(mutex *mutex)
{
	return !IMP_pthread_mutex_trylock(&mutex->mutex);
}

inline void mutex_unlock(mutex *mutex)
{
	IMP_pthread_mutex_unlock(&mutex->mutex);
}

inline void mutex_destroy(mutex *mutex)
{
	mutex_unlock(mutex);
	IMP_pthread_mutex_destroy(&mutex->mutex);
}

inline u32 thread_get_id()
{
	return (u32)syscall(__NR_gettid);
}

inline void thread_sleep(u64 microseconds)
{
	usleep(microseconds);
}

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  	// for sleep() , darwin alias c
#include <errno.h>		// define EBUSY system flag
#include <time.h>
#include <assert.h>

typedef struct my_struct_tag
{
	pthread_mutex_t 	mutex;
	pthread_cond_t		cond;
	int 				value;
}my_struct_t;

my_struct_t data = {
	PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0
};

int hibernation = 1;

void* wait_thread(void* arg)
{
	int status;
	sleep(hibernation);
	status = pthread_mutex_lock(&data.mutex);
	assert(status == 0);

	data.value = 1;

	status = pthread_cond_signal(&data.cond);
	assert(status == 0);

	status = pthread_mutex_unlock(&data.mutex);
	assert(status == 0);
	return NULL;
}

int main(int argc, char const *argv[])
{
	int status;
	pthread_t wait_thread_id;
	struct timespec timeout;

	/*
	If an argument is sepecified, interpret it as the number
	of seconds for wait_thread to sleep before signaling the
	condition variable. You can play with this to see the
	condition wait below time out of wake normaly.
	*/
	if (argc > 1)
	{
		hibernation = atoi (argv[1]);
	}

	/*
	Create wait_thread.
	*/
	status = pthread_create(&wait_thread_id, NULL, wait_thread, NULL);
	assert(status == 0);

	/*
	Wait on the condition variable for 2 seonds, or util
	signaled by the wait_thread. Normally, wait_thread
	should signal. If you raise "hibernation" above 2 
	seconds, it will time out.
	*/
	timeout.tv_sec = time(NULL) + 2;
	timeout.tv_nsec = 0;
	status = pthread_mutex_lock(&data.mutex);
	assert(status == 0);

	while(data.value == 0 ) {
		/* When in condition waiting, the thread will auto release mutex lock. */
		status = pthread_cond_timedwait(&data.cond, &data.mutex, &timeout);  
		if (status == ETIMEDOUT)
		{
			printf("Condition wait timed out. \n");
			break;
		}
		else 
		{
			assert(status == 0);
		}
	}

	if (data.value != 0)
	{
		printf("Condition was signaled.\n");
	}
	status = pthread_mutex_unlock(&data.mutex);
	assert(status == 0);
	return 0;
}
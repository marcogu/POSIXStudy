#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  	// for sleep() , darwin alias c
#include <errno.h>		// define EBUSY system flag

#define SPIN 1000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long counter;
time_t end_time;

/*
Thread start routine that repeatedly locks a mutex and
increments a counter.
*/
void* counter_thread(void * arg) 
{
	int status;
	int spin;

	/*
	Unitl end_time, increment the counter each seond. Instead of 
	just incrementing the counter, it sleeps for anther second
	with the mutex locked, to give monitor_thread a reasonable
	chance of running.
	*/
	while(time(NULL) < end_time)
	{
		status = pthread_mutex_lock(&mutex);
		if (status != 0)
		{
			// check error state here.
		}

		for (spin= 0; spin < SPIN; spin++)
		{
			counter++;
		}

		status = pthread_mutex_unlock(&mutex);
		//check status value
		sleep(1);
	}
	printf("Counter  is %#1lx \n", counter);
	return NULL;
}

/*
Thread start routine to "monitor" the counter. Every 3
seconds, try to lock the mutex and read the counter. If the
trylock fails, skip this cycle.
*/
void* monitor_thread(void *arg)
{
	int status;
	int misses = 0;

	/* Loop until end_time, checking the counter every 3 seonds */
	while(time(NULL) < end_time	)
	{
		sleep(3);
		status = pthread_mutex_trylock(&mutex);
		if (status != EBUSY)
		{
			printf("Counter is %ld\n", counter/SPIN);
			status = pthread_mutex_unlock(&mutex);
		}
		else
		{
			misses++;
		}
	}
	printf("monitor Thread missed update %d times.\n", misses);
	return NULL;
}

int main(int argc, char const *argv[])
{
	int status;
	pthread_t counter_thread_id;
	pthread_t monitor_thread_id;

	end_time = time(NULL) + 60;
	status = pthread_create(&counter_thread_id, NULL, counter_thread, NULL);
	status = pthread_create(&monitor_thread_id, NULL, monitor_thread, NULL);

	status = pthread_join(counter_thread_id, NULL);
	status = pthread_join(monitor_thread_id, NULL);
	return 0;
}

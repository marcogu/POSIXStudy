#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  	// for sleep() , darwin alias c
#include <errno.h>		// define EBUSY system flag
#include <time.h>

/*
Initialize a work queue.
*/
int workq_init (workq_t* wq, int threads, void(*engine)(void*arg))
{
	int status;
	status = pthread_attr_init(&wq->attr);
	// check status value
	status = pthread_attr_setdetachstate(&wq->attr, PTHREAD_CREATE_DETACHED);
	if (status != 0)
	{
		pthread_attr_destroy(&wq->attr);
		return status;
	}
	status = pthread_mutex_init(&wq->mutex, NULL);
	if (status != 0)
	{
		pthread_attr_destroy(&wq->attr);
		return status;
	}
	status = pthread_cond_init(&wq->cv, NULL);
	if (status != 0)
	{
		pthread_mutex_destroy(&wq->mutex);
		pthread_attr_destroy(&wq->attr);
		return status;
	}
	wq->quit = 0;
	wq->first = wq->last = NULL;
	wq->parallelism = threads;
	wq->counter = 0;
	wq->idle = 0;
	wq->engine = engine;
	wq->valid = WORKQ_VALID;
	return 0;
}




#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>		// fprintf()
#include <unistd.h>  	// for sleep() , darwin alias c
#include <errno.h>		// define EBUSY system flag

#define ITERATIONS 10

/*
Initialize a static array of 3 mutexes.
*/
pthread_mutex_t mutex[3] = {
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER,
	PTHREAD_MUTEX_INITIALIZER
};

int backoff = 1;		/*Whether to backoff or deadlock */
int yield_flag = 0;	/* 0: no yield, >0: yield, <0: sleep */

/*
This is a thread routine that locks all mutexes in 
order, to ensure a conflict with lock_reverse, which does the 
opposite.
*/
void* lock_forward(void*arg)
{
	int i, iterate, backoffs;
	int status;

	for (iterate = 0; iterate < ITERATIONS; iterate++)
	{
		backoffs = 0;
		for (int i = 0; i < 3; ++i)
		{
			if (i==0)
			{
				status = pthread_mutex_lock(&mutex[i]);
				// check status value
			}
			else
			{
				if (backoff)
				{
					status = pthread_mutex_trylock(&mutex[i]);
				}
				else
				{
					status = pthread_mutex_lock(&mutex[i]);
				}

				if (status == EBUSY)
				{
					backoffs++;
					DPRINTF(("(forward locker backing off at %d)\n", i));
					for (; i >=0; i--)
					{	
						status = pthread_mutex_unlock(&mutex[i]);
					}
				}
				else
				{
					DPRINTF(("(forward locker got %d)\n", i));
				}
			}

			/* 
			Yield processor, if needed to be sure locks get 
			interleaved on a uniprocessor.
			*/
			if (yield_flag)
			{
				if (yield_flag>0)
				{
					sched_yield();
				}
				else
				{
					sleep(1);
				}
			}
		}

		/*
		Report that we got 'em, and unlock to try again.'
		*/
		printf("lock forward got all locks, %d backoffs \n", backoffs);
		pthread_mutex_unlock(&mutex[2]);
		pthread_mutex_unlock(&mutex[1]);
		pthread_mutex_unlock(&mutex[0]);
		sched_yield();
	}
	return NULL;
}

/*
This is a thread start routine that locks all mutexes in 
reverse order, to ensure a conflict with lock_forward, which
does the opposite.
*/
void *lock_backward(void*arg)
{
	int i, iterate, backoffs;
	int status;
	for (iterate = 0; iterate < ITERATIONS; iterate++)
	{
		backoffs = 0;
		for (i = 2; i >= 0; i--)
		{
			if (i == 2)
			{
				status = pthread_mutex_lock(&mutex[i]);
			}
			else
			{
				if (backoff)
				{
					status = pthread_mutex_trylock(&mutex[i]);
				}
				else 
				{
					status = pthread_mutex_lock(&mutex[i]);
				}
				
			}
		}
	}
}


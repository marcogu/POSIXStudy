#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  	// for sleep() , darwin alias c
#include <errno.h>		// define EBUSY system flag

/*
Define a structure, with a mutex and condition variable.
*/
typedef struct my_struct_tag
{
	pthread_mutex_t mutex;
	pthread_cond_t	cond;
	int value;
} my_struct_t;

int main(int argc, char const *argv[])
{
	my_struct_t *data;
	int status;

	data = malloc( sizeof (my_struct_t ));
	assert(data != NULL);
	status = pthread_mutex_init(&data->mutex, NULL);
	assert(status == 0);
	status = pthread_cond_init(&data->cond, NULL);
	assert(status == 0);
	status = pthread_cond_destory(&data->cond);
	assert(status == 0);
	status = pthread_mutex_destory(&data->mutex);
	assert(status == 0);
	free(data);
	return status;
}
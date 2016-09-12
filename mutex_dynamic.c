#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

/**
	Define a structure, with a mutex
*/

typedef struct my_structure_tag
{
	pthread_mutex_t mutex;
	int value;
} my_structure_t;	

int main(int argc, char const *argv[])
{
	my_structure_t* data;
	int status;

	data = malloc(sizeof(my_structure_t));
	if (data == NULL)
	{
		printf("%s\n", "data can not be NULL");
	}

	status = pthread_mutex_init(&data->mutex, NULL);
	if (status != 0)
	{
		printf("%s\n", "Destroy mutex");
	}

	(void)free(data);
	return status;
}
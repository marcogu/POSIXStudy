#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  	// for sleep() , darwin alias c
#include <errno.h>		// define EBUSY system flag

#define CLIENT_THREADS	4

#define REQ_READ	1
#define REQ_WRITE	2
#define REQ_QUIT	3

/*
Internal to server "package" -- one for each request.
*/
typedef struct request_tag
{
	struct request_tag	*next;
	int 				operation;
	int 				synchronous;
	int 				done_flag;
	pthread_cond_t		done;
	char				prompt[32];
	char				text[120];
}request_t;

/*
Static context for the server
*/
typedef struct tty_server_tag
{
	request_t 			*first;
	request_t 			*last;
	int 				running;
	pthread_mutex_t		mutex;
	pthread_cond_t		request;
} tty_server_t;

tty_server_t tty_server = {
	NULL, NULL, 0,
	PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER
};

/*
Main program data
*/
int client_threads;
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t clients_done = PTHREAD_COND_INITIALIZER;

/*
The server start routine. It waits for a request to appear
in tty_server. requests using the request condition variable.
It process requests in FIFO order. If a request is marked
"synchronous" (synchronous != 0), the server will set done_flag
and signal the request's condition variable. The client is 
responsible for freeing the request. If the request wat not
synchronous, the server will free the request on completion.
*/
void * tty_server_routine(void*arg)
{
	static pthread_mutex_t prompt_mutex = PTHREAD_MUTEX_INITIALIZER;
	request_t * request;
	int operation, len;
	int status;

	while(1){
		status = pthread_mutex_lock(&tty_server.mutex);
		// check status value is zerso.
		/* Wait for data */
		while(tty_server.first == NULL){
			status = pthread_cond_wait (&&tty_server.request, &tty_server.mutex);
			// check status value is zerso.
		}
		request = tty_server.first;
		tty_server.first = request->next;
		if (tty_server.first == NULL)
		{
			tty_server.last = NULL;
		}
		status = pthread_mutex_unlock(&tty_server.mutex);
		//check status value is zerso.

		/*
		Process the data
		*/
		operation = request->operation;
		switch(operation){
			case REQ_QUIT:
				break;
			case REQ_READ:
				if (strlen(request->prompt) > 0)
				{
					printf(request->prompt);
				}
				if (fgets(request->text, 128, stdin) == NULL)
				{
					request->text[0] = '\0';
					/*
					Because fgets returns the newline, and we don't
					want it, we look for it, and turn it into a null 
					(truncating the input) if found. It should be the
					last character, if it is there.
					*/
					len = strlen(request->text);
					if (len>0 && request->text[len-1] == '\n')
					{
						request->text[len-1]='\0';
					}
				}
				break;
			case REQ_WRITE:
				puts (request->text);
				break;	

		}

	}

}



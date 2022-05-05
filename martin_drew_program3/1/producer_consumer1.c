#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>

/* based off of the provided code in the assignment sheet */
/* design to have 5 slots */
const int BUFFER_SIZE = 5;
int counter = 0; /* initialize counter */

/* the producer */
void* producer(void* params)
{
	/* initialize */
    int* buffer = (int*) params;
	int in = 0;
	int next_produced = 0;
	/* while true */
	while(1)
	{
		if (next_produced > 1000000)
		{
			/* max integer of the sequence */
			pthread_exit(0); /* call pthread exit */
		}
		else
		{
			/* increase next produced */
			next_produced++;
		}
		while(counter == BUFFER_SIZE);
		/* does nothing */
		buffer[in] = next_produced;
		in = (in + 1) % BUFFER_SIZE;
		counter++;
	}
}
/* the consumer */
void* consumer(void* params)
{
	/* initialize */
    int* buffer = (int*) params;
	int out = 0;
	int check_expected = 1;
	while(1)
	{
		while(counter = 0);
		/* does nothing */
		int next_consumed = buffer[out];
		out = (out + 1) % BUFFER_SIZE;
		counter--;
		/* check if expected */
		if (next_consumed == check_expected)
		{
			check_expected++;
		}
		else
		{
			/* print the error */
			printf("ERROR: The consumer recieved the sequence up to %d correctly. The next number (incorrect) number was %d\n", check_expected-1, next_consumed);
			exit(1);
		}
	}
	/* exit */
	exit(0);
}
/* main method for threads */
int main()
{
	int buffer[BUFFER_SIZE];
	/* pthread_t one to produce and one to consume */
	pthread_t produce;
	pthread_t consume;

	/* create threads */
	pthread_create(&produce, NULL, &producer, &buffer);
	pthread_create(&consume, NULL, &consumer, &buffer);
	/* join threads */
	pthread_join(produce, NULL);
	pthread_join(consume, NULL);
	/* finished */
	return 0;
}


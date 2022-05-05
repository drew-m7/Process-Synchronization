#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
/* include buffer header file */
#include "buffer.h"

/* this program fixes race condition seen in question 1 */
/* I have modified question 1 to meet requirements for this program */
/* creates producer and consumer threads to remove and insert items into buffer */
/* avoids race condition using mutex and semaphores */
/* utilized some of the provided code in the assignment sheet */
/* Drew Martin */

/* asssume size of word list wordsEn.txt */
const int LIST_LENGTH = 109587; /* slightly larger just in case to be safe */
buffer_item buffer[BUFFER_SIZE];
/* initialize */
int counter;
int in;
int out;
int length;
/* must use mutex and semaphore */
pthread_mutex_t mutex;
/* also need semaphore empty and full according to requirements */
sem_t empty, full;

/* array for wordsEn.txt */
char** wordsEnArray;

/* remove an item */
int remove_item(buffer_item *item)
{
    sem_wait(&full);
    /* lock mutex */
    pthread_mutex_lock(&mutex);

    int returnInt; /* integer to return from function, should be 0 always */

    if (counter > 0) 
    {
        *item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        counter--;
        returnInt = 0;

    }

    /* unlock mutex and empty sem */
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    /*return 0 */
    return returnInt;

}

/* insert an item */
int insert_item(buffer_item item)
{
    /* semaphore empty */
    sem_wait(&empty);
    /*lock mutex */
    pthread_mutex_lock(&mutex);
    int returnInt; /* integer to return from function, sh0uld be 0 */

    if (counter < BUFFER_SIZE)
    {
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        counter++;
        returnInt = 0;
    }
    /* unlock mutex */
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    /* return 0 */
    return returnInt;
}

/* producer to insert item */
void* producer(void* params)
{
    /* the buffer item*/
    buffer_item item;
    /* while true */
    while(1)
    {
        sleep(rand() % 5); /* between 0 and 5 seconds */
        int index = (rand() % length); /* index from words array */
        item = wordsEnArray[index];
        if (insert_item(item) == 0)
        {
            /* to id thread, I used pthread_self which simply returns the ID of the calling thread */
            printf("[Producer thread ID: %d] inserted an item (word: %s) to the buffer\n", (int)pthread_self(), item); /* output result */
        }
        
    }
}

/* consumer to remove item */
void* consumer(void* params)
{
    /* the buffer item */
    buffer_item item;
    /* while true */
    while(1)
    {
        sleep(rand() % 5); /* between 0 and 5 seconds */
        if (remove_item(&item) == 0) /* remove an item */
        {
            printf("[Consumer thread ID: %d] removed an item (word: %s) from the buffer\n", (int)pthread_self(), item); /* output result */
        }
    }
}

/* main method */
int main(int argc, char const *argv[])
{
    /* must accept 4 args */
    /* % ./producer_consumer2 sleep_length num_producer num_consumer */
    if (argc == 4)
    {
         /* --------------------- */
        /* get the three arguments from command line */
        int TIME = atoi(argv[1]);
        int PRODUCERS = atoi(argv[2]);
        int CONSUMERS = atoi(argv[3]);
        /* ----------------------- */

        /* read in the arguments and read wordsEn.txt */
        FILE *theFile;
        length = 0;
        int buffer[BUFFER_SIZE];
        /* allocate memory for words */
        wordsEnArray = malloc(LIST_LENGTH * sizeof(char*));
        for (int x=0; x<LIST_LENGTH; x++){
            wordsEnArray[x] = malloc(sizeof(char) * 30);
        }
        /* open wordsEn.txt */
        theFile = fopen("wordsEn.txt", "r");
        while (fgets(wordsEnArray[length], sizeof(char)* 30, theFile) != NULL & length < LIST_LENGTH)
        {
            length++;
        }

        /* now need to add the pthreads */
        /* putting them into separate prod and consumer thread arrays */
        pthread_t theProducers[PRODUCERS];
        pthread_t theConsumers[CONSUMERS];
        /* set to 0 */
        in = 0;
        out = 0;
        counter = 0;

        /* need to initialize the semaphores(empty and also full) and mutex */
        sem_init(&full, 0 , 0);
        sem_init(&empty,0,BUFFER_SIZE);
        pthread_mutex_init(&mutex, NULL);

        /* now create the pthreads for producer and consumer */
        for (int x = 0; x<PRODUCERS; x++)
        {
            /* create each producer thread */
            pthread_create(&theProducers[x], NULL, &producer, NULL);
        }
        for (int x = 0; x<CONSUMERS; x++)
        {
            /* create each consumer thread */
            pthread_create(&theConsumers[x], NULL, &consumer, NULL);
        }

        /* sleep, should be for 180 according to assignment sheet */
        /* sleep can be whatever is specified at command line */
        sleep(TIME);
        /* when sleeping is finished, update user on what has occured */
        if (PRODUCERS == 0)
        {
            /* there are no producers created so, nothing can be consumed */
            printf("All consumer threads are blocked since no data is produced by producer threads!\n");
        }
        if (CONSUMERS == 0)
        {
            /* no consumers are created so producers will eventually get blocked */
            printf("All producer threads are eventually blocked since there is no consumer threads!\n");
        }
        if (PRODUCERS >0 && CONSUMERS >0)
        {
            /* there are producers and consumers specified, so program will run until sleep time done */
            printf("\nSuccess! Sleep time has completed, and program is going to exit!\n\n");
        }

        return 0;
        /* finished */
        

    }

    else {
        /* didn't enter correct number of arguments */
        printf("The correct number of args was not used, should be 3 args after the executable!\n");
        exit(1); /* incorrect args, exit */
    }
}
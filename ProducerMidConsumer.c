#include <stdlib.h>
#include <stdbool.h> /* required for true, false keywords */
#include <stdio.h> /* required for rand() */
#include <unistd.h> /* required for sleep */
#include <semaphore.h> /* required for empty and full semaphores */
#include <pthread.h> /* required for threads and mutex */
#include "buffer.h"

#define SLEEP_RAND_MAX 3 //
#define BUFFER_RAND_MAX 40

int getRandomNumber(int MAX);

/*

|=====|     |======|     |======|     |======|     |======|
|  P  |---->| BUF1 |---->|  M   |---->| BUF2 |---->|   C  |
|=====|     |======|     |======|     |======|     |======|

*/


// declare variables for first shared buffer
buffer_item* buffer1[BUFFER_SIZE];
char* queue1="Producer/Middleman";
int in1=0, out1=0, counter1=0;
// Create Mutex, empty, full
sem_t full1, empty1; //semaphores
pthread_mutex_t mutex1; //mutex

// declare variables for second shared buffer
buffer_item* buffer2[BUFFER_SIZE];
char* queue2 = "Middleman/Consumer";
int in2=0, out2=0, counter2=0;
sem_t full2, empty2; // semaphores
pthread_mutex_t mutex2; // mutex

pthread_attr_t attr; // Set of attributes for thread

/* 
Display contents of the buffer 
Example output:: 
[empty][5][4][27] in = 4, out = 1, queue = Middleman/Consumer
*/
void display(buffer_item* buffer, char* queue, int in, int out) {
    for(int i=0; i<BUFFER_SIZE; i++) {
        if (buffer[i]==0) {
            printf("[empty]");
        } else {
            printf("[%d]", buffer[i]);
        }
    }
    printf(" in = %d,", in);
    printf(" out = %d,", out);
    printf(" queue = %s\n\n", queue);
}

// Synchronization while inserting items into First shared buffer
int insert_item1(buffer_item item) {
	// If the buffer is full do not insert item
	if(counter1 == BUFFER_SIZE) {
		sleep(2);
	}

    // Acquire locks
	pthread_mutex_lock(&mutex1); // mutex
	sem_wait(&empty1); // empty

	//If the buffer is not full add item and increment the buffer counter
	if (counter1 < BUFFER_SIZE) {
        counter1++;
		buffer1[in1] = item; //Insert the item in to the buffer
		printf("Insert_item inserted item %d at position %d\n", item, in1);
		in1=(in1+1) % BUFFER_SIZE;
		display(buffer1, queue1, in1, out1);
	}

    // Release locks
	pthread_mutex_unlock(&mutex1);  // mutex 
	sem_post(&full1); // full

}

// Synchronization while removing items from First shared buffer
int remove_item1(buffer_item *item) {
	//If the  buffer has no elements then there is nothing to remove.
	if(counter1 == 0) {
		sleep(2);
	}
    // Acuire locks
	pthread_mutex_lock(&mutex1); // mutex 
	sem_wait(&full1); // full 

	// Remove the item and decrement the buffer counter.
	if (counter1 > 0) {
		counter1--;
		*item=buffer1[out1];
		buffer1[out1]=0;

		printf("Remove_item removed item %d at position %d\n", *item, out1);
		out1 = (out1+1) % BUFFER_SIZE;

		display(buffer1, queue1, in1, out1);			
	}
    // Release locks
	pthread_mutex_unlock(&mutex1); // mutex
	sem_post(&empty1); // empty
}

// Synchronization while inserting items into Second shared buffer
int insert_item2(buffer_item item) {
	// If the buffer is full do not insert item
	if(counter2 == BUFFER_SIZE) {
		sleep(2);
	}

    // Acquire locks
	pthread_mutex_lock(&mutex2); //mutex
	sem_wait(&empty2); // empty

	//If the buffer is not full add item and increment the buffer counter
	if (counter2 < BUFFER_SIZE) {
        counter2++;
		buffer2[in2] = item; //Insert the item in to the buffer
		printf("Insert_item inserted item %d at position %d\n", item, in1);
		in2=(in2+1) % BUFFER_SIZE;
		display(buffer2, queue2, in2, out2);
	}

    // Release locks
	pthread_mutex_unlock(&mutex2);  // mutex 
	sem_post(&full2); // full

}

// Synchronization while removing items from Second shared buffer
int remove_item2(buffer_item *item) {
	//If the  buffer has no elements then there is nothing to remove.
	if(counter2==0) {
		sleep(2);
	}

    // Acquire locks
	pthread_mutex_lock(&mutex2); // mutex 
	sem_wait(&full2); // full 

	// Remove the item and decrement the buffer counter.
	if (counter2 > 0) {
		counter2--;
		*item=buffer2[out2];
		buffer2[out2]=0;

		printf("Remove_item removed item %d at position %d\n",*item, out1);
		out2 = (out2+1) % BUFFER_SIZE;

		display(buffer2, queue2, in2, out2);			
	}
    // Release locks
	pthread_mutex_unlock(&mutex2); // mutex
	sem_post(&empty2); // empty
}

void *producer(void *param) {
    buffer_item item;

    while (true) {
        int threadSleepTime = getRandomNumber(3);
        printf("Producer thread %lu sleeping for %d seconds\n", pthread_self(), threadSleepTime);
        sleep(threadSleepTime);

        item = getRandomNumber(40);
        printf("Producer produced %d item\n", item);

        if (insert_item1(item) < 0) {
            printf("Producer error\n");
        } else {
            printf("Producer thread %lu inserted value %d\n", pthread_self(), item);
        }
    }
    // Exit producer thread
    pthread_exit(NULL);
}

void *middleman(void *param) {
    buffer_item item;

    while (true) {
        int threadSleepTime = getRandomNumber(3);
        printf("Middleman thread %lu sleeping for %d seconds\n", pthread_self(), threadSleepTime);
        sleep(threadSleepTime);

        if (remove_item1(&item) < 0) {
            printf("Middleman error\n");
        } else {
            printf("Middleman thread %lu removed value %d\n", pthread_self(), item);
        }

        if (insert_item2(item) < 0) {
            printf("Middleman error\n");
        } else {
            printf("Middleman thread %lu inserted value %d\n", pthread_self(), item);
        }
    }
    // Exit middleman thread
    pthread_exit(NULL);
}


void *consumer(void *param) {
    buffer_item item;

    while (true) {
        int threadSleepTime = getRandomNumber(3);
        printf("Consumer thread %lu sleeping for %d seconds\n", pthread_self(), threadSleepTime);
        sleep(threadSleepTime);

        if (remove_item2(&item) < 0) {
            printf("Consumer error\n");
        } else {
            printf("Consumer thread %lu removed value %d\n", pthread_self(), item);
        }
    }
    // Exit consumer thread
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

    // seed random number generation for reproducibility
    srand(time(NULL));

    int sleepTime;
    int numProducers, numMiddleMen, numConsumers;

    /* Validate and Get cmd line args */
    switch (argc){
        case 5:
            // process command line arguments
            sleepTime = atoi(argv[1]);
            numProducers = atoi(argv[2]);
            numMiddleMen = atoi(argv[3]);
            numConsumers = atoi(argv[4]);
            printf("Sleep Time: %d\tNum of producers: %d\tNum of MiddleMen: %d\tNum of consumers: %d\n",
                sleepTime, numProducers, numMiddleMen, numConsumers);
            break;

        default:
            printf("USAGE: Enter sleep time, no. of producers, no. of middlemen, no. of consumers \n");
            exit(0);
            break;
    }

    printf("\nMain thread beginning\n\n");
   // The _init methods in pthread or sem are used to initialize the threads
   // If attr is specified as NULL, all attributes are set to the default mutex attributes for the newly created mutex.
    pthread_mutex_init(&mutex1, NULL);
    /*
    An indication to the system of how the semaphore is going to be used. 
    A value of zero indicates that the semaphore will be used only by threads within the current process. 
    A nonzero value indicates that the semaphore may be used by threads from other processes.
    */
    sem_init(&empty1, 0, BUFFER_SIZE);
    sem_init(&full1, 0, 0);

    pthread_mutex_init(&mutex2, NULL);
    sem_init(&empty2, 0, BUFFER_SIZE);
    sem_init(&full2, 0, 0);
    /* Initialize buffer */
    /* create producer threads */
    pthread_t producer_threads[numProducers];
    pthread_t middleman_threads[numMiddleMen];
    pthread_t consumer_threads[numConsumers];

    for(int i=0; i < numProducers; i++) {
        int rc = pthread_create(&producer_threads[i], &attr, producer, NULL);
        if (rc) {
            printf("ERROR: Return Code from pthread_create() is %d\n", rc);
            exit(1);
        }
        printf("Creating producer thread with id %lu \n", producer_threads[i]);
    }
    printf("Producer threads created successfully...\n\n");

    /* create middleman threads */
    for(int i=0; i < numMiddleMen; i++) {
        int rc = pthread_create(&middleman_threads[i], &attr, middleman, NULL);
        if (rc) {
            printf("ERROR: Return Code from pthread_create() is %d\n", rc);
            exit(1);
        }
        printf("Creating middleman thread with id %lu \n", middleman_threads[i]);
    }
    printf("Middleman threads created successfully...\n\n");

    /* create consumer threads */
    for(int i=0; i < numConsumers; i++) {
        int rc = pthread_create(&consumer_threads[i], &attr, consumer, NULL);
        if (rc) {
            printf("ERROR: Return Code from pthread_create() is %d\n", rc);
            exit(1);
        }
        printf("Creating consumer thread with id %lu \n", consumer_threads[i]);
    }
    printf("Consumer threads created successfully...\n\n");

    printf("Main thread sleeping for %d seconds\n", sleepTime);
    sleep(sleepTime);

    /* kill threads and exit */
    pthread_mutex_destroy(&mutex1);
    sem_destroy(&empty1);
    sem_destroy(&full1);
    // 
    pthread_mutex_destroy(&mutex2);
    sem_destroy(&empty2);
    sem_destroy(&full2);

    printf("Main thread exiting.\n");
    exit(0);
}

/************************************************
 * Generates a random integer between 1 and MAX *
 ************************************************/
int getRandomNumber(int MAX){
     int n;
     n = (rand() % MAX) + 1;
     return n;
}

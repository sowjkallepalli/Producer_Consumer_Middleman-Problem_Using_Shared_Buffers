# Producer_Consumer_Middleman-Problem_Using_Shared_Buffers
 Multi-threaded producer – middleman - consumer environment using shared buffers.

Background:

Producer-Consumer

|=====|     |======|     |======|     |======|
|  P  |---->| BUF1 |---->| BUF2 |---->|   C  |
|=====|     |======|     |======|     |======|


This problem is an extension of the standard bounded-buffer problem, which is a traditional synchronization problem. Typically, we have one producer process and one consumer process, which share a buffer. The producer writes items into the buffer and the consumer reads items back out. In a multi-threaded environment, we cannot assume the order in which the producer and consumer run, and since they share a data structure and possibly more variables, we must use synchronization mechanisms in order to avoid race conditions.

Note that the busy waiting does not enforce mutual exclusion. Rather it ensures that a producer does not try to write into a full buffer and the consumer does not try to read from an empty one. Since busy waiting is inefficient, we will use semaphores to provide mutual exclusion and to control access to empty/full buffers. Semaphores allow us to put a lock around the critical section (the code where shared variables are updated.) Here’s a solution to the bounded buffer problem using semaphores:
First we need the shared variables:
1 shared buffer, which can hold N items
A semaphore mutex initialized to the value 1 (Note: mutex is a binary semaphore providing mutual exclusion)
A semaphore full initialized to the value 0
A semaphore empty initialized to the value N (Note: full and empty are counting semaphores)

Producer-MiddleMan-Consumer

|=====|     |======|     |======|     |======|     |======|
|  P  |---->| BUF1 |---->|  M   |---->| BUF2 |---->|   C  |
|=====|     |======|     |======|     |======|     |======|


In this problem, we have extended the standard bounded-buffer to add another shared buffer and a middleman. The producer will write items into the producer buffer. The middleman will take items from the producer buffer and place them in the consumer buffer. The consumer will read items out of the consumer buffer. So, the producer and the middleman share the producer buffer, and the middleman and the consumer share the consumer buffer.
We will implement the producer, middleman, and consumer as threads in one process. This makes sharing of data structures easy as threads share their data address space. We will also be using the synchronization mechanisms from the Pthreads library including mutex locks and semaphore locks.


Here is the skeleton for main():
int main(int argc, char *argv[]) {
/* 1. Get command line arguments */ 
/* 2. Initialize buffer */
/* 3. Create producer threads. */
/* 4. Create middleman threads. */
/* 5. Create consumer threads. */
/* 6. Sleep. */
} /* 7. Kill threads and exit. */

Main will take 4 arguments from the command line:
1. The length of time the program should run. 
2. The number of producer threads.
2. The number of middleman threads.
3. The number of consumer threads.



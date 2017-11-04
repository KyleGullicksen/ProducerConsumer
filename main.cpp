/*
 * -Solve the bounded-buffer problem (using the producer and the consumer processes in Figs 5.9 and 5.10)
 * -Use 3 semaphores: full and empty (counts of the # of full and empty slots, respectively, in the buffer)
 * -The third is a mutex (binary semaphore)
 *      -Use a mutex lock instead of a actual binary semaphore
 *
 *  pthread tutorial: http://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html
 */


#include <vector>
#include <iostream>
#include <stdlib.h>
#include <pthread.h>

using namespace std;


int main()
{
    pthread_t thread1, thread2;
}
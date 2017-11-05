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

#define DEFAULT_THREAD_ATTRIBUTES NULL

void *producer(void * params);
void *consumer(void * params);
void initBuffer(vector<int> &buffer);

vector<int> buffer;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;
int producerIndex = 0;
int consumerIndex = 0;
bool go = false;

int main()
{
    pthread_t producerThread, consumerThread;

    //Create the producer and consumer threads
    pthread_create(&producerThread, DEFAULT_THREAD_ATTRIBUTES, producer, NULL);
    pthread_create(&consumerThread, DEFAULT_THREAD_ATTRIBUTES, consumer, NULL);

    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    exit(EXIT_SUCCESS);
}


void *producer(void * params)
{
   while(producerIndex < 100)
   {
       while(go);
       pthread_mutex_lock(&lock);
       buffer.push_back(counter);
       counter++;
       producerIndex++;
       cout << "Producer: Counter value: " << counter << endl;
       pthread_mutex_unlock(&lock);
   }

    go = true;

}

void *consumer(void * params)
{
    consumerIndex = 99;

    while(consumerIndex > -1)
    {
        while(!go);
        pthread_mutex_lock(&lock);
        int erasedVal = buffer[consumerIndex];
        buffer.erase(buffer.begin() + consumerIndex);
        consumerIndex--;
        cout << "Consumer: Erased value: " << erasedVal << endl;
        pthread_mutex_unlock(&lock);
    }
}


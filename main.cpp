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
#include <unistd.h>
#include <math.h>

using namespace std;
#define DEFAULT_THREAD_ATTRIBUTES NULL
#define BUFFER_SIZE 5
#define MAX_THREADS 8

typedef long buffer_item;

struct CommandLineOptions
{
    int sleepTimeSeconds = 0;
    int consumerThreads = 1;
    int producerThreads = 1;
};
void* producer(void* params);
void* consumer(void* params);
int insert_item(buffer_item& item);
buffer_item remove_item();
CommandLineOptions commandLineOptions(int argc, char** argv);
void init(vector<pthread_t>& threads, int threadCount);
int createThreads(vector<pthread_t>& threads, const pthread_attr_t* attr, void* (* start_routine)(void*), void* arg);
int getSleepTime();

//Globals
buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int index = 0;
pthread_cond_t producerConditional;
pthread_cond_t consumerConditional;

int main(int argc, char** argv)
{
    CommandLineOptions options = commandLineOptions(argc, argv);

    //Avoid massacring the poor CPU
    if((options.producerThreads + options.consumerThreads) > MAX_THREADS)
        options.consumerThreads = options.producerThreads = MAX_THREADS / 2;

    vector<pthread_t> producers(options.producerThreads);
    vector<pthread_t> consumers(options.consumerThreads);

    init(producers, options.producerThreads);
    init(consumers, options.consumerThreads);

    createThreads(producers, DEFAULT_THREAD_ATTRIBUTES, producer, nullptr);
    createThreads(consumers, DEFAULT_THREAD_ATTRIBUTES, consumer, nullptr);

    sleep(options.sleepTimeSeconds);
    exit(EXIT_SUCCESS);
}

void init(vector<pthread_t>& threads, int threadCount)
{
    for(int count = 0; count < threadCount; ++count)
    {
        pthread_t thread;
        threads.push_back(thread);
    }
}

int createThreads(vector<pthread_t>& threads, const pthread_attr_t* attr, void* (* start_routine)(void*), void* arg)
{
    for(pthread_t thread : threads)
        pthread_create(&thread, attr, start_routine, arg);
}

/*
 * 3 options:
 * 1) How long to sleep for
 * 2) The number of producer threads
 * 3) The number of consumer threads
 *
 * <prog_name> -s <x> -pt <x1> -ct <x2> [last two optional]
 */

CommandLineOptions commandLineOptions(int argc, char** argv)
{
    CommandLineOptions options;
    switch(argc)
    {
        case 7:
            options.consumerThreads = stoi(argv[6]);
        case 5:
            options.producerThreads = stoi(argv[4]);
        case 3:
            options.sleepTimeSeconds = stoi(argv[2]);
            break;
        default:
            cout << "Invalid command line arguments" << endl;
    }
    return options;
}

void* producer(void* params)
{
    pthread_mutex_lock(&lock);
    while(index >= BUFFER_SIZE)
        pthread_cond_wait(&producerConditional, &lock);

    buffer_item randomVal = random();
    insert_item(randomVal);
    cout << "Producer: Inserting " << randomVal << endl;

    pthread_cond_signal(&consumerConditional);
    pthread_mutex_unlock(&lock);

    int sleepTime = getSleepTime();
    cout << "Producer: Sleeping for: " << sleepTime << endl;
    sleep(sleepTime);
}

void* consumer(void* params)
{
    pthread_mutex_lock(&lock);
    while(index <= 0)
        pthread_cond_wait(&consumerConditional, &lock);

    buffer_item removedItem = remove_item();
    cout << "Consumer: Removed " << removedItem << endl;

    pthread_cond_signal(&producerConditional);
    pthread_mutex_unlock(&lock);

    int sleepTime = getSleepTime();
    cout << "Consumer: Sleeping for: " << sleepTime << endl;
    sleep(sleepTime);
}

int getSleepTime()
{
    return (rand() % 10) + 1;
}

int insert_item(buffer_item& item)
{
    if(index >= BUFFER_SIZE)
        index = 0;
    buffer[index] = item;
    ++index;
}

buffer_item remove_item()
{
    if(index <= 0)
        index = BUFFER_SIZE - 1;
    else
        index--;

    buffer_item item = buffer[index];
    return item;
}

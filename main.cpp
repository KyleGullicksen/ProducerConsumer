/*
 * Created by Kyle Gullicksen and Benjamin Siegel
 * Date Written: 11/10/17 - 11/22/17
 * Course: CS433
 * Assignment #4
 *
 * main.cpp
 *
 *
 *
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

//Glorious Functions
void* producer(void* params);
void* consumer(void* params);
int insert_item(buffer_item& item);
buffer_item remove_item();
CommandLineOptions commandLineOptions(int argc, char** argv);
void init(vector<pthread_t>& threads, int threadCount);
int createThreads(vector<pthread_t>& threads, const pthread_attr_t* attr, void* (* start_routine)(void*), void* arg);
int getSleepTime();
string bufferToStr();

//Globals
buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int index = 0;
pthread_cond_t producerConditional;
pthread_cond_t consumerConditional;

/*main
 *Parameters(int argc, char** argv)
 * Creates and implements the vectors, init, and threads used in the Product-Consumer problem
 */
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

/*initialize threadCount
 *Parameters: vector<pthread_t>& threads, int threadCount
 * This function takes the thread from pthread_t and pushes it onto the threads integer.
 */
void init(vector<pthread_t>& threads, int threadCount)
{
    for(int count = 0; count < threadCount; ++count)
    {
        pthread_t thread;
        threads.push_back(thread);
    }
}

/*createThreads
 * Parameters: vector<pthread_t>& threads, const pthread_attr_t* attr, void* (* start_routine)(void*), void* arg
 * Creates the threads using the parameters passed into the function
 */
int createThreads(vector<pthread_t>& threads, const pthread_attr_t* attr, void* (* start_routine)(void*), void* arg)
{
    for(pthread_t thread : threads)
        pthread_create(&thread, attr, start_routine, arg);
}

/*CommandLineOptions
 *
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

/*Producer
 *Parameter: void* params
 * Implementation of the producer function
 * Produces a random item and then locks the mutux
 */
void* producer(void* params)
{
    pthread_mutex_lock(&lock);
    while(index >= BUFFER_SIZE)
        pthread_cond_wait(&producerConditional, &lock);

    buffer_item randomVal = random();
    insert_item(randomVal);
    cout << "Item " << randomVal << " inserted by a producer. The current content of the buffer is " << bufferToStr() << "." << endl;

    pthread_cond_signal(&consumerConditional);
    pthread_mutex_unlock(&lock);

    int sleepTime = getSleepTime();
    sleep(sleepTime);
}

/*Consumer
 * Parameter: void* params
 * Implementation of the consumer function
 * Removes the random item and unlocks the mutex.
 *
 *
 */
void* consumer(void* params)
{
    pthread_mutex_lock(&lock);
    while(index <= 0)
        pthread_cond_wait(&consumerConditional, &lock);

    buffer_item removedItem = remove_item();
    cout << "Item " << removedItem << " removed by a consumer. The current content of the buffer is " << bufferToStr() << "." << endl;

    pthread_cond_signal(&producerConditional);
    pthread_mutex_unlock(&lock);

    int sleepTime = getSleepTime();
    sleep(sleepTime);
}

/*getSleepTime
 * returns a random number for sleep
 */
int getSleepTime()
{
    return (rand() % 10) + 1;
}

/*insert_item
 * Parameter: buffer_item& item
 * Inserts the item into the buffer
 */
int insert_item(buffer_item& item)
{
    if(index >= BUFFER_SIZE)
        index = 0;
    buffer[index] = item;
    ++index;
}

/*remove_item
 *Removes the item from the buffer
 */
buffer_item remove_item()
{
    if(index <= 0)
        index = BUFFER_SIZE - 1;
    else
        index--;

    buffer_item item = buffer[index];
    return item;
}

/*bufferToStr
 * Converts the buffer to a string so that it can be
 * outputted in the console.
 *
 */
string bufferToStr()
{
    string str = "[";
    bool isFirst = true;

    for(int bufferIndex = 0; bufferIndex < index; ++bufferIndex)
    {
        if(!isFirst)
        {
            str.push_back(',');
            str.push_back(' ');
        }

        isFirst = false;
        str.append(std::to_string(buffer[bufferIndex]));
    }
    str.push_back(']');

    if(str == "[]")
        return "[empty]";
    else
        return str;
}
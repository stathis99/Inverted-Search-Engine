#ifndef JOBSCHEDULER
#define JOBSCHEDULER

#include <pthread.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include "include/core.h"

typedef void (*thread_funct)(void *args);

//arguments for jobs, used by function
typedef struct args{
    int *activeThreads;

    DocID doc_id;

    char* doc_str;
}Arguments;

//job struct
typedef struct job{
    //pointer to the function to be executed
    thread_funct func;

    //pointer to a struct containing arguments
    void *args;
}Job;

//queue node for jobs list
typedef struct queueNode{
    //pointer to job
    Job *job;

    //pointer to next job node
    struct queueNode *next;
}queueNode;

//FIFO queue to store jobs
typedef struct Queue{
    //pointer to the head
    queueNode *head;

    //pointer to the tail
    queueNode *tail;

    //number of elements inside
    int counter;

}Queue;

//job scheduler struct
typedef struct JobScheduler{
    //number of avaiable threads
    int execution_threads;
    
    //FIFO queue to hold the jobs
    Queue *q;

    //array with the ids of the avaiable threads
    pthread_t* tids;

    //mutex for all locking
    pthread_mutex_t work_mutex;

    //signals that there is work to be processed
    pthread_cond_t work_cond;

    //signals that there are no threads processing
    pthread_cond_t working_cond;

    pthread_cond_t work_done;

    int last_doc;

    size_t alive_thread_count;

    size_t thread_count;

    //false by default, turns to 1 to stop threads' execution
    bool stop;
}JobScheduler;


//--------------JobScheduler related functions--------------//

//create and return the queue
Queue* create_queue();

//create and return a queue node
queueNode* create_queue_node(Job *);

//add a job to the end of the queue
void queue_insert(JobScheduler *, Job *);

//destroys queue node and the job
void free_queue_node(queueNode* n);

//pop first job inserted
Job* queue_pop(Queue *);

//function that initialises and returns job scheduler 
JobScheduler* initialize_jobScheduler(int);

//initialize and return a job
Job* create_job(thread_funct function, Arguments* arg_struct);

//wait for all tasks to finish 
int wait_all_tasks_finish(JobScheduler* sch);

int execute_all_jobs(JobScheduler* sch);

#endif
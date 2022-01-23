#ifndef JOBSCHEDULER
#define JOBSCHEDULER

#include <pthread.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include "include/core.h"

//arguments for jobs
typedef struct args{
    int *activeJobs;

    DocID doc_id;

    char* doc_str;

    //1 if document, 0 if query
    int is_doc;

    QueryID query_id;

    MatchType match_type;

    unsigned int match_dist;

    char * query_str;
}Arguments;

typedef void (*thread_funct)(void *args);
//job struct
typedef struct job{
    //function to be executed
    thread_funct func;

    //function's arguments to be passed
    void *args;
}Job;

//queue node for jobs list
typedef struct queueNode{
    Job *job;
    struct queueNode *next;
}queueNode;

//FIFO queue to store jobs
typedef struct Queue{
    queueNode *tail;
    queueNode *head;
    int counter;
}Queue;

//job scheduler struct
typedef struct JobScheduler{
    //FIFO queue to hold the jobs
    Queue *q;

    //number of scheduler threads, set at initialization
    int execution_threads;

    //number of alive threads
    size_t alive_thread_count;

    size_t thread_count;

    //thread id's
    pthread_t* tids;

    //mutex for accessing c.s.
    pthread_mutex_t js_mutex;

    //wake threads, signal that there is work to do
    pthread_cond_t workToDo_cond;

    //signal that we have no more threads that can take jobs at the moment
    pthread_cond_t working_cond;

    //signal that we have processed a batch, used for wait_all_tasks_finish
    pthread_cond_t work_done;

    //used to break while(1) that threads are running on
    int last_doc;

    //0 by default, turns to 1 to stop threads' execution
    int stop;

}JobScheduler;


//JobScheduler related functions

//function that initialises and returns job scheduler 
JobScheduler* initialize_jobScheduler(int num_threads);

//what every thread will do to take and process jobs
void* thread_function(void* jobSch);

//create and return the queue
Queue* create_queue();

//create and return a queue node
queueNode* create_queue_node(Job* job);

//destroys queue node and the job
void free_queue_node(queueNode* n);

//initialize and return a job
Job* create_job(thread_funct funct, Arguments* arguments);

//add a job to the end of the queue
void submit_job(JobScheduler *, Job* job);

//pop first job inserted
Job* pop_job(Queue* q);

//wait for all tasks to finish 
int wait_all_tasks_finish(JobScheduler* sch);

int execute_all_jobs(JobScheduler* sch);

#endif
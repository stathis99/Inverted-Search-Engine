#include <stdio.h>
#include <stdlib.h>
#include "jobScheduler.h"

//jobScheduler constructor
JobScheduler* initialize_jobScheduler(int num_threads){

    JobScheduler* ptr = NULL;
    ptr = malloc(sizeof(JobScheduler));
    if(ptr == NULL){
        perror("malloc");
        exit(-1);
    }

    ptr->execution_threads = num_threads;
    ptr->thread_count = num_threads;
    ptr->alive_thread_count = 0;

    ptr->q = create_queue();

    ptr->stop = 0;
    ptr->last_doc = 0;

    //num_threads creation
    ptr->tids = malloc(sizeof(pthread_t)*num_threads);

    //initialize mutexes
    pthread_mutex_init(&(ptr->js_mutex),NULL);
    pthread_cond_init(&(ptr->workToDo_cond),NULL);
    pthread_cond_init(&(ptr->working_cond),NULL);
    pthread_cond_init(&(ptr->work_done),NULL);

    for(int i =0; i <num_threads;i++){
        pthread_create(&ptr->tids[i], NULL,thread_function, ptr);
        pthread_detach(ptr->tids[i]);
    }

    return ptr;
}

//create and return the queue
Queue* create_queue(){

    Queue* q = NULL;
    q = malloc(sizeof(Queue));
    
    if(q == NULL){
        perror("malloc");
        exit(-1);
    }

    q->tail = NULL;
    q->head = NULL;
    q->counter = 0;

    return q;
}

//create and return a queue node
queueNode* create_queue_node(Job* j){

    queueNode *node = NULL;
    
    node = malloc(sizeof(queueNode));

    if(node == NULL){
        perror("malloc");
        exit(-1);
    }
    node->job = j;

    return node;
}

void free_queue_node(queueNode* n){
    free(n);
}

//initialize and return a job
Job* create_job(thread_funct funct, Arguments* arguments){

    Job* ptr = malloc(sizeof(Job));
    ptr->func = funct;
    ptr->args = arguments;

    return ptr;

}


//add a job to the end of the queue
void submit_job(JobScheduler* js, Job* j){

    queueNode* node = NULL;

    //create a job
    node = create_queue_node(j);

    pthread_mutex_lock(&(js->js_mutex));

    if(js->q->counter == 0){      //queue empty, add first
        js->q->tail = node;
        js->q->head = node;
        js->q->counter = 1;
    }else{      //not empty, add after tail
        js->q->tail->next = node;
        js->q->tail = js->q->tail->next; 
        js->q->counter++; 
    }
    
    //wake threads
    pthread_cond_broadcast(&(js->workToDo_cond));
    pthread_mutex_unlock(&(js->js_mutex));

}

//pop first job inserted
Job* pop_job(Queue* q){

    queueNode *node = NULL;

    //no jobs to be had 
    if(q->counter == 0){
        return NULL;
    }
    q->counter--;

    node = q->head;
    Job *j = NULL;
    j = node->job;

    //head is now the next node
    q->head = q->head->next;

    free_queue_node(node);

    return j;    
}

void* thread_function(void* jobSch){
    JobScheduler *JS = (JobScheduler *) jobSch;
    Job *work;

    while(!JS->last_doc){
        pthread_mutex_lock(&(JS->js_mutex));
        while(JS->q->counter == 0 && !JS->last_doc){

            pthread_cond_wait(&(JS->workToDo_cond), &(JS->js_mutex));
        }

        if(JS->last_doc && JS->q->counter == 0){  
            break;
        }

        work = pop_job(JS->q);
        JS->alive_thread_count++;
        pthread_mutex_unlock(&(JS->js_mutex));

        if(work != NULL){
            work->func(work->args);
            //destroy job
            //Arguments* arguments = work->args;
            //free(arguments->doc_str);
            //free(arguments);
            free(work);
        }
        
        pthread_mutex_lock(&(JS->js_mutex));
        JS->alive_thread_count--;
        if(JS->q->counter == 0 && JS->alive_thread_count == 0){
            pthread_cond_broadcast(&(JS->work_done));
            if(JS->stop == 1 || JS->last_doc == 1){
                break;
            }

        }
        if(JS->stop == 1 || JS->last_doc == 1){
            //pthread_cond_broadcast(&(JS->work_done));
            break;
        }

        pthread_mutex_unlock(&(JS->js_mutex));
    }

    JS->thread_count--;

    pthread_mutex_unlock(&(JS->js_mutex));
    
    return NULL;
}

int wait_all_tasks_finish(JobScheduler* sch){
    pthread_mutex_lock(&(sch->js_mutex));
    while(sch->q->counter != 0){
        pthread_cond_wait(&(sch->work_done), &(sch->js_mutex));
    }
    pthread_mutex_unlock(&(sch->js_mutex));
    return 1;
}

int execute_all_jobs(JobScheduler* sch){
    return 1;
}
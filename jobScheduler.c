#include <stdio.h>
#include <stdlib.h>
#include "jobScheduler.h"


//--------------JobScheduler related functions--------------//
extern int flag;
//create and return the queue
Queue* create_queue(){

    Queue* q = NULL;

    //allocate space
    q = malloc(sizeof(Queue));
    //error check
    if(q == NULL){
        perror("malloc");
        exit(-3);
    }

    //set pointers to null
    q->head = NULL;
    q->tail = NULL;
    //set counter to 0
    q->counter = 0;

    //return the object
    return q;
}

//create and return a queue node
queueNode* create_queue_node(Job* j){

    queueNode *node = NULL;
    
    //allocate space
    node = malloc(sizeof(queueNode));
    //error check
    if(node == NULL){
        perror("malloc");
        exit(-3);
    }

    //add the job
    node->job = j;

    //return the object
    return node;
}

//destroys queue node and the job
void free_queue_node(queueNode* n){

    //free job

    //free node
    free(n);

}

//add a job to the end of the queue
void queue_insert(JobScheduler* JS, Job* j){

    queueNode* node = NULL;

    //create a node for the job
    node = create_queue_node(j);

    //lock while insertion
    //its critical section
    pthread_mutex_lock(&(JS->work_mutex));

    //if the queue is empty
    if(JS->q->counter == 0){
        //update tail and head
        JS->q->head = node;
        JS->q->tail = node;
        //adjust counter
        JS->q->counter = 1;

    //add the node to the end of the queue
    }else{

        JS->q->tail->next = node;
        JS->q->tail = JS->q->tail->next; 
        //adjust counter
        JS->q->counter += 1; 

    }
    
    //notify the threads
    pthread_cond_broadcast(&(JS->work_cond));
    //end of critical section
    pthread_mutex_unlock(&(JS->work_mutex));

}

//pop first job inserted
Job* queue_pop(Queue* q){

    queueNode *node = NULL;
    Job *j = NULL;

    //if the list is empty its en error
    if(q->counter == 0){
        return NULL;
    }

    //adjust the counter
    q->counter -= 1;

    //keep the node
    node = q->head;
    //keep the job
    j = node->job;

    //move head to next node
    q->head = q->head->next;

    //free the node
    free_queue_node(node);

    return j;    
}

//initialize and return a job
Job* create_job(thread_funct function, Arguments* arg_struct){

    // error check
    if(function == NULL){
        printf("Null function\n");
        return NULL;
    }

    Job* ptr = malloc(sizeof(Job));
    ptr->func = function;
    ptr->args = arg_struct;

    return ptr;

}


void* thread_Job_function(void* jobSch){

    JobScheduler* JS = (JobScheduler*) jobSch;
    Job *work;

    while(1){
        // lock mutex to secure the job scheduler numbers
        pthread_mutex_lock(&(JS->work_mutex));
        //check if there is more work to be done
        //wait
        while(JS->q->counter == 0){
            //lock work cond and unlock work mutex
            //printf("Thread going to sleep\n");
            // if(JS->last_doc == 1){   //corner case, thread gets stuck here
            //     JS->thread_count--;
            //     //pthread_cond_signal(&(JS->working_cond));
            //     pthread_mutex_unlock(&(JS->work_mutex));
            //     pthread_cond_broadcast(&(JS->work_done));
            //     printf("end\n");
            //     return NULL;
            // }
            pthread_cond_wait(&(JS->work_cond), &(JS->work_mutex));
        }

        //if(JS->stop){
        // if(JS->q->counter == 0){printf("eh?\n");
        //     //wake StartQuery
        //     pthread_cond_broadcast(&(JS->work_done));
        //     break;
        // }

        work = queue_pop(JS->q);
        JS->alive_thread_count++;
        pthread_mutex_unlock(&(JS->work_mutex));

        if(work != NULL){
            printf("%d\n\n",pthread_self());
            work->func(work->args);
            //destroy job
            free(work);
        }

        if(JS->q->counter == 0){
            //wake StartQuery
            //pthread_cond_broadcast(&(JS->work_done));
            
            //are we done? break and terminate thread
            // pthread_mutex_lock(&(JS->work_mutex));
            // while(JS->q->counter == 0){

            // }
            // pthread_mutex_unlock(&(JS->work_mutex));
            pthread_mutex_lock(&(JS->work_mutex));
            pthread_cond_broadcast(&(JS->work_done));
            if(JS->last_doc == 1){printf("Nothing else\n");
                pthread_mutex_unlock(&(JS->work_mutex));
                break;
            }
            pthread_mutex_unlock(&(JS->work_mutex));
        }
        pthread_mutex_lock(&(JS->work_mutex));
        JS->alive_thread_count--;
        
        if(!JS->stop && JS->alive_thread_count == 0 && JS->q->counter == 0){
            pthread_cond_signal(&(JS->working_cond));
        }
        pthread_mutex_unlock(&(JS->work_mutex));
    }

    JS->thread_count--;
    pthread_cond_signal(&(JS->working_cond));
    pthread_mutex_unlock(&(JS->work_mutex));
    printf("end\n");

    return NULL;
}

//jobScheduler constructor
JobScheduler* initialize_jobScheduler(int num_threads){

    JobScheduler* ptr = NULL;
    ptr = malloc(sizeof(JobScheduler));
    if(ptr == NULL){
        exit(-3);
    }
    ptr->q = create_queue();
    ptr->execution_threads = num_threads;
    ptr->alive_thread_count = 0;
    ptr->thread_count = num_threads;
    ptr->stop = 0;
    ptr->last_doc = 0;
    // void* temp;

    // Arguments* args = malloc(sizeof(Arguments));
    // args->argv = malloc(sizeof(int));
    // int i = 555;
    // args->argv[0] = i;
    // Job* job = create_job(NULL,args);
    // queue_insert(ptr->q,job);
   
    //make n numThreads in array
    ptr->tids = malloc(sizeof(pthread_t)*num_threads);

    // init mutex
    pthread_mutex_init(&(ptr->work_mutex),NULL);
    pthread_cond_init(&(ptr->work_cond),NULL);
    pthread_cond_init(&(ptr->working_cond),NULL);
    pthread_cond_init(&(ptr->work_done),NULL);

    for(int i =0; i <num_threads;i++){
        //make thread
        pthread_create(&ptr->tids[i], NULL,thread_Job_function, ptr);
        //pthread_detach(ptr->tids[i]);
    }

    printf("JobScheduler initialized\n");
    return ptr;
}


int wait_all_tasks_finish(JobScheduler* sch){
    pthread_mutex_lock(&(sch->work_mutex));
    while(sch->q->counter != 0){
        pthread_cond_wait(&(sch->work_done), &(sch->work_mutex));
    }
    pthread_mutex_unlock(&(sch->work_mutex));
    return 1;
}

int execute_all_jobs(JobScheduler* sch){
    
}


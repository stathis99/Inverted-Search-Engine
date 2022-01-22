#include  "../include/acutest.h"			
#include "../include/core.h"
#include "./tests_struct.h"
#include "../jobScheduler.h"
#include <pthread.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;

//do one test for jobsched initialization
//one for a few matchdocuments

void worker(void *arg)
{
	
    Arguments *args = arg;
	*(args->activeThreads) -= 1;
	

	//signal
	pthread_mutex_lock(&mu);
  	pthread_cond_signal(&cond);
  	pthread_mutex_unlock(&mu);
	
	

}

void test_JobScheduler()
{

    JobScheduler *jb;
    Job *job;
    Arguments *args;

	int num = 10;

	args = malloc(sizeof(Arguments));
	args->activeThreads = &num;


    jb = initialize_jobScheduler(1);

    for (int i = 0; i<1; i++) {
        
        job = create_job(worker,args);
  
        submit_job(jb,job);
    }
    
	//wait worker to run
	pthread_mutex_lock(&mu);
	pthread_cond_wait(&cond,&mu);
	pthread_mutex_unlock(&mu);

	jb->stop = 1;
	jb->alive_thread_count = 0;
	
 
	
	free(args);
	free(jb->tids);
	free(jb->q);
	free(jb);

	

	TEST_ASSERT(num == 9);

}


TEST_LIST = {
	{"Hamming Distance",hamming_distance_test},
	{"Edit Distance",edit_distance_check},
	{"Deduplicate Document: List Created",doc_dedupl_string},
	{"Deduplicate Document: Number of Records",doc_dedupl_num},
	{"Deduplicate Document: 'Born' Word Records",doc_dedupl_born},
	{"Query Hash Table",query_handling},
    {"Query Words in Structs",query_structs_handling},
    {"test_JobScheduler",test_JobScheduler},
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};

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
    pthread_mutex_lock(&mu);
	
     Arguments *args = arg;
     *(args->activeJobs)  -= 1;
     printf("\nworker %lu gets job %d from queue \n",pthread_self(),*(args->activeJobs));	
	
     pthread_mutex_unlock(&mu);	

}

void test_JobScheduler()
{
        JobScheduler *jb;
   	Job *job;
    	Arguments *args;

       jb = initialize_jobScheduler(2);


//create 3 jobs and add them to job scheduler
    for(int i=0; i<3;i++){


    	args = malloc(sizeof(Arguments));
	args->activeJobs = &i;

        
      job = create_job(worker,args);
  
      submit_job(jb,job);

    }
    
	wait_all_tasks_finish(jb);

	
        int mytest = *(args->activeJobs);


	jb->stop = 1;
	jb->alive_thread_count = 0;
	
 
	
	free(args);
	free(jb->tids);
	free(jb->q);
	free(jb);

	
	//jobs must be 0
	TEST_ASSERT(mytest == 0);

}


TEST_LIST = {
    {"test_JobScheduler",test_JobScheduler},
	{ NULL, NULL } // Ο„ΞµΟΞΌΞ±Ο„Ξ―Ξ¶ΞΏΟ…ΞΌΞµ Ο„Ξ· Ξ»Ξ―ΟƒΟ„Ξ± ΞΌΞµ NULL
};

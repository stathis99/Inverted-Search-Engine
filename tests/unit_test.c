#include  "../include/acutest.h"			
#include "../include/core.h"
#include "./tests_struct.h"
#include "../jobScheduler.h"
#include <pthread.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;

char temp[MAX_DOC_LENGTH];

void doc_dedupl_string(){
	FILE* fp = fopen("./tests/doc_dedupl.txt","r");
    if(fp == NULL){
        printf("Couldnt open file.\n");
        exit(-1);
    }

	//read the whole document
    if(EOF==fscanf(fp, "%[^\n\r] ", temp)){
		printf("Corrupted Test File at Read Document.\n");
		exit(-1);
	}

	Words_Hash_Table* words_hash_table_test = NULL;
	//initialize our hash table
    words_hash_table_test = malloc(sizeof(Words_Hash_Table));
    for(int i=0; i< WORD_HASH_TABLE_BUCKETS; i++){
        words_hash_table_test->words_hash_buckets[i] = NULL;
    }

	const word* read_word = strtok(temp, " ");
	while(read_word != NULL){
		int hash = djb2(read_word) % WORD_HASH_TABLE_BUCKETS;
        if(words_hash_table_test->words_hash_buckets[hash] != NULL){

            int found = -1;
            Words_Hash_Bucket* current = words_hash_table_test->words_hash_buckets[hash];
            Words_Hash_Bucket* previous = NULL;
            while(current != NULL){
                if(strcmp(current->this_word,read_word) == 0){
                    //word exists, skip
                    found = 1;
                    break;
                }
                previous = current;
                current = current->next;
            }
            if( found == -1){
                //add it to the hash buckets, search it in structs
                previous->next = malloc(sizeof(Words_Hash_Bucket));
                strcpy(previous->next->this_word,read_word);
                previous->next->next = NULL;
            }
		}else{
            //create first bucket
            words_hash_table_test->words_hash_buckets[hash] = malloc(sizeof(Words_Hash_Bucket));
            strcpy(words_hash_table_test->words_hash_buckets[hash]->this_word,read_word);
            words_hash_table_test->words_hash_buckets[hash]->next = NULL ;
        }

		read_word = strtok(NULL, " ");
	}
	
	char our_words[MAX_DOC_LENGTH] = "";
	for(int i=0; i < WORD_HASH_TABLE_BUCKETS; i++){
		Words_Hash_Bucket* current = words_hash_table_test->words_hash_buckets[i];
		while(current != NULL){
			char temp[33];
			strcpy(temp,current->this_word);
			strcat(temp," ");
			strcat(our_words,temp);

			current = current->next;
		}
	}

	TEST_CHECK(strcmp(our_words, "hockey ethan diablo player cody born writer coen ") == 0);

    for(int i=0; i< WORD_HASH_TABLE_BUCKETS; i++){
        while(words_hash_table_test->words_hash_buckets[i] != NULL){
            Words_Hash_Bucket* temp = words_hash_table_test->words_hash_buckets[i];
            words_hash_table_test->words_hash_buckets[i] = words_hash_table_test->words_hash_buckets[i]->next;
            free(temp);
        }
    }
    free(words_hash_table_test);
    words_hash_table_test = NULL;
	fclose(fp);
}

void doc_dedupl_num(){
	FILE* fp = fopen("./tests/doc_dedupl.txt","r");
    if(fp == NULL){
        printf("Couldnt open file.\n");
        exit(-1);
    }

	//read the whole document
    if(EOF==fscanf(fp, "%[^\n\r] ", temp)){
		printf("Corrupted Test File at Read Document.\n");
		exit(-1);
	}
	Words_Hash_Table* words_hash_table_test = NULL;
	//initialize our hash table
    words_hash_table_test = malloc(sizeof(Words_Hash_Table));
    for(int i=0; i< WORD_HASH_TABLE_BUCKETS; i++){
        words_hash_table_test->words_hash_buckets[i] = NULL;
    }

	const word* read_word = strtok(temp, " ");
	while(read_word != NULL){
		int hash = djb2(read_word) % WORD_HASH_TABLE_BUCKETS;
        if(words_hash_table_test->words_hash_buckets[hash] != NULL){

            int found = -1;
            Words_Hash_Bucket* current = words_hash_table_test->words_hash_buckets[hash];
            Words_Hash_Bucket* previous = NULL;
            while(current != NULL){
                if(strcmp(current->this_word,read_word) == 0){
                    //word exists, skip
                    found = 1;
                    break;
                }
                previous = current;
                current = current->next;
            }
            if( found == -1){
                //add it to the hash buckets, search it in structs
                previous->next = malloc(sizeof(Words_Hash_Bucket));
                strcpy(previous->next->this_word,read_word);
                previous->next->next = NULL;
            }
		}else{
            //create first bucket
            words_hash_table_test->words_hash_buckets[hash] = malloc(sizeof(Words_Hash_Bucket));
            strcpy(words_hash_table_test->words_hash_buckets[hash]->this_word,read_word);
            words_hash_table_test->words_hash_buckets[hash]->next = NULL ;
        }

		read_word = strtok(NULL, " ");
	}

	int total = 0;

	for(int i=0; i < WORD_HASH_TABLE_BUCKETS; i++){
		Words_Hash_Bucket* current = words_hash_table_test->words_hash_buckets[i];
		while(current != NULL){
			total++;
			current = current->next;
		}
	}

	TEST_CHECK(total == 8 );

	fclose(fp);

    for(int i=0; i< WORD_HASH_TABLE_BUCKETS; i++){
        while(words_hash_table_test->words_hash_buckets[i] != NULL){
            Words_Hash_Bucket* temp = words_hash_table_test->words_hash_buckets[i];
            words_hash_table_test->words_hash_buckets[i] = words_hash_table_test->words_hash_buckets[i]->next;
            free(temp);
        }
    }
    free(words_hash_table_test);
    words_hash_table_test = NULL;

}

void doc_dedupl_born(){
	FILE* fp = fopen("./tests/doc_dedupl.txt","r");
    if(fp == NULL){
        printf("Couldnt open file.\n");
        exit(-1);
    }

	//read the whole document
    if(EOF==fscanf(fp, "%[^\n\r] ", temp)){
		printf("Corrupted Test File at Read Document.\n");
		exit(-1);
	}
	Words_Hash_Table* words_hash_table_test = NULL;
	//initialize our hash table
    words_hash_table_test = malloc(sizeof(Words_Hash_Table));
    for(int i=0; i< WORD_HASH_TABLE_BUCKETS; i++){
        words_hash_table_test->words_hash_buckets[i] = NULL;
    }

	const word* read_word = strtok(temp, " ");
	while(read_word != NULL){
		int hash = djb2(read_word) % WORD_HASH_TABLE_BUCKETS;
        if(words_hash_table_test->words_hash_buckets[hash] != NULL){

            int found = -1;
            Words_Hash_Bucket* current = words_hash_table_test->words_hash_buckets[hash];
            Words_Hash_Bucket* previous = NULL;
            while(current != NULL){
                if(strcmp(current->this_word,read_word) == 0){
                    //word exists, skip
                    found = 1;
                    break;
                }
                previous = current;
                current = current->next;
            }
            if( found == -1){
                //add it to the hash buckets, search it in structs
                previous->next = malloc(sizeof(Words_Hash_Bucket));
                strcpy(previous->next->this_word,read_word);
                previous->next->next = NULL;
            }
		}else{
            //create first bucket
            words_hash_table_test->words_hash_buckets[hash] = malloc(sizeof(Words_Hash_Bucket));
            strcpy(words_hash_table_test->words_hash_buckets[hash]->this_word,read_word);
            words_hash_table_test->words_hash_buckets[hash]->next = NULL ;
        }

		read_word = strtok(NULL, " ");
	}

    int born_count = 0;
	
	for(int i=0; i < WORD_HASH_TABLE_BUCKETS; i++){
		Words_Hash_Bucket* current = words_hash_table_test->words_hash_buckets[i];
		while(current != NULL){

            if(strcmp("born",current->this_word) == 0){
                born_count++;
            }
			current = current->next;
		}
	}

    TEST_CHECK(born_count == 1 );
	fclose(fp);

    for(int i=0; i< WORD_HASH_TABLE_BUCKETS; i++){
        while(words_hash_table_test->words_hash_buckets[i] != NULL){
            Words_Hash_Bucket* temp = words_hash_table_test->words_hash_buckets[i];
            words_hash_table_test->words_hash_buckets[i] = words_hash_table_test->words_hash_buckets[i]->next;
            free(temp);
        }
    }
    free(words_hash_table_test);
    words_hash_table_test = NULL;

}

void query_handling(){
		FILE* fp = fopen("./tests/dedupl.txt","r");
    if(fp == NULL){
        printf("Couldnt open file.\n");
        exit(-1);
    }
	int fres;
	char ch;
	int id;

    //initialize query hash table
    InitializeIndex_QueryTest();

	while(1){
        fres = fscanf(fp, "%c %u ", &ch, &id);
        if(fres == EOF){
            break;
        }
		if(ch == 's'){
            int match_type;
			int match_dist;

			if(EOF == fscanf(fp, "%d %d %*d %[^\n\r] ", &match_type, &match_dist, temp)){
				printf("Corrupted Test File at Read Queries.\n");
				exit(-1);
			}

            if(StartQuery_QueryTest(id, temp, match_type, match_dist) == EC_FAIL){
                printf("StartQuery failed for QueryID %d\n",id);
                exit(-1);
            }
        }
	}
	
	extern Query_Hash_Table* Q_Hash_Table_Test;
    printf("\n");
    for(int i=0; i<QUERY_HASH_BUCKETS_TEST; i++){
        Query* current = Q_Hash_Table_Test->query_hash_buckets[i];
        if(current != NULL){
            printf("Printing index %d\n",i);
            while(current != NULL ){
                printf("%d %s ->",current->query_id,current->str);
                current = current->next;
            }
            printf("\n");
        }
    }

	fclose(fp);
    DestroyIndex_QueryTest();
}

void hamming_distance_test(){
	char word1[] = "edits";
	char word2[] = "maximum";
	char word3[] = "giant";
	char word4[] = "extremm";
	char word5[] = "miximum";
	TEST_CHECK(hamming_dist(word1,word2,strlen(word1),strlen(word2)) == -1);
	TEST_CHECK(hamming_dist(word1,word3,strlen(word1),strlen(word3)) == 5);
	TEST_CHECK(hamming_dist(word2,word4,strlen(word2),strlen(word4)) == 6);
	TEST_CHECK(hamming_dist(word5,word2,strlen(word5),strlen(word2)) == 1);
	TEST_CHECK(hamming_dist(word5,word1,strlen(word5),strlen(word1)) == -1);
}

void edit_distance_check(){
	char word1[] = "edits";
	char word2[] = "maximum";
	char word3[] = "giant";
	char word4[] = "extremm";
	char word5[] = "miximum";
	TEST_CHECK(distance(word1,strlen(word1),word2,strlen(word2)) != -1);
	TEST_CHECK(distance(word1,strlen(word1),word2,strlen(word2)) == 6);
	TEST_CHECK(distance(word2,strlen(word2),word4,strlen(word4)) == 6);
	TEST_CHECK(distance(word5,strlen(word5),word2,strlen(word2)) == 1);
	TEST_CHECK(distance(word5,strlen(word5),word1,strlen(word1)) == 6);

}

void query_structs_handling(){
	FILE* fp = fopen("./tests/dedupl.txt","r");
    if(fp == NULL){
        printf("Couldnt open file.\n");
        exit(-1);
    }
	int fres;
	char ch;
	int id;

    //initialize query hash table
    InitializeIndex_Insert();

	while(1){
        fres = fscanf(fp, "%c %u ", &ch, &id);
        if(fres == EOF){
            break;
        }
		if(ch == 's'){
            int match_type;
			int match_dist;

			if(EOF == fscanf(fp, "%d %d %*d %[^\n\r] ", &match_type, &match_dist, temp)){
				printf("Corrupted Test File at Read Queries.\n");
				exit(-1);
			}

            if(StartQuery_QueryTest_Insert(id, temp, match_type, match_dist) == EC_FAIL){
                printf("StartQuery failed for QueryID %d\n",id);
                exit(-1);
            }
        }
	}


    print_hash_tables_test();
	fclose(fp);
    DestroyIndex_Insert();
}

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

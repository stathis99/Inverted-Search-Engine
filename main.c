#include <stdio.h>
#include <stdlib.h>
#include "./include/structs.h"
#include <string.h>
#include <time.h> 

//array used to pass all query words
char temp[MAX_DOC_LENGTH];

int main(int argc, char* argv[]){
    extern query_ids* queries_head;

    //clock added
    double time_spent = 0.0;
    clock_t begin = clock();

    //initialize all our global structures
    if(InitializeIndex() != EC_SUCCESS){
        printf("InitializeIndex failed.\n");
        exit(-1);
    }

    //FILE* fp = fopen("./files/queries.txt","r");
    FILE* fp = fopen("./files/small_test.txt","r");
    if(fp == NULL){
        printf("Couldnt open file.\n");
        exit(-1);
    }

    //read and store all queries
    char ch;
	unsigned int id;
    int fres;

    //start processing queries
    while(1){
        fres = fscanf(fp, "%c %u ", &ch, &id);
        if(fres == EOF){
            break;
        }
		if(ch == 's'){
            //printf("Reading query %d\n",id);
            int match_type;
			int match_dist;

			if(EOF == fscanf(fp, "%d %d %*d %[^\n\r] ", &match_type, &match_dist, temp)){
				printf("Corrupted Test File at Read Queries.\n");
				exit(-1);
			}

            //start query
            if(StartQuery(id, temp, match_type, match_dist) == EC_FAIL){
                printf("StartQuery failed for QueryID %d\n",id);
                exit(-1);
            }
        }else if(ch == 'm'){
            
            if(EOF==fscanf(fp, "%*u %[^\n\r] ", temp)){
				printf("Corrupted Test File at Read Document.\n");
				exit(-1);
			}
		    ErrorCode err = MatchDocument(id, temp);

        }else if(ch == 'r'){
            unsigned int num_res=0;
			if(EOF==fscanf(fp, "%d ", &num_res)){
                return 1;
		    }
            query_ids* temp = queries_head;

            int qid;

			for(int i=0;i<(int)num_res;i++)
			{
				if(EOF==fscanf(fp, "%u ", &qid)){
					printf("Corrupted Test File.\n");
					fflush(NULL);
					return 1;
				}

                if(temp->queryID == qid){
                        
                        temp = temp->next;
                    
                }else{
                    printf("Found difference in document %d",id);
                }
				
                // printf("%d ->", qid);
			}

        }else if(ch=='e')
		{
			ErrorCode err=EndQuery(id);

			if(err==EC_FAIL)
			{
				printf("The call to EndQuery() returned EC_FAIL.\n");
				fflush(NULL);
				return 1;
			}
			else if(err!=EC_SUCCESS)
			{
				printf("The call to EndQuery() returned unknown error code.\n");
				fflush(NULL);
				return 1;
			}
		}
    }

    //close input file
    fclose(fp);

    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("The elapsed time is %f seconds\n", time_spent);
    DestroyIndex();

   return 0;
}
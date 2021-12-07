#include <stdio.h>
#include <stdlib.h>
#include "./include/structs.h"
#include <string.h>
#include <time.h> 

//array used to pass all query words
char temp[MAX_DOC_LENGTH];

int main(int argc, char* argv[]){
    
    //clock added
    double time_spent = 0.0;
    clock_t begin = clock();

    //initialize all our global structures
    if(InitializeIndex() != EC_SUCCESS){
        printf("InitializeIndex failed.\n");
        exit(-1);
    }

    FILE* fp = fopen("./files/queries.txt","r");
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
			//ErrorCode err=MatchDocument(id, temp,hash_tables_edit);
        }else{
            break;
        }
		
    }
print_query_list();
    //printing of each struct follows
    //print_hash_table_exact(hash_tables_exact);

    //print_hash_tables(hash_tables_edit);
    //print_bk_tree(ix,0);

    // print_hash_tables(hash_tables_hamming);
    // for(int i=0; i<=28;i++){
    //     print_bk_tree(hamming_root_table[i],0);
    // }

    //Edit distance structures free'd
    //delete_hash_tables_edit(hash_tables_edit);
    //destroy_entry_index(&ix);

    //Hamming distance structures free'd
    //delete_hash_tables_hamming(hash_tables_hamming,hamming_root_table);

    //Exact match structure free'd
    // delete_hash_tables_exact(hash_tables_exact);

    //close input file
    fclose(fp);

    clock_t end = clock();
    time_spent += (double)(end - begin) / CLOCKS_PER_SEC;
    printf("The elapsed time is %f seconds\n", time_spent);

   // if(argc != 3){
   //    printf("Invalid number of arguments given\n");
   //    exit(-1);
   // }

   // enum match_type my_match_type;
   // //choose EDIT_DIST or HAMMING_DIST
   // if(atoi(argv[1]) == 1){
   //    my_match_type = EDIT_DIST;
   // }else if(atoi(argv[1]) == 2){
   //    my_match_type = HAMMING_DIST;
   // }else{
   //    printf("Invalid match type given\n");
   //    exit(-1);
   // }
   // //chose a threshold 
   // int threshold = atoi(argv[2]);

   // int number = 0;

   // //open the file we are reading from
   // FILE* fp = fopen("./files/queries.txt","r");
   // if(fp == NULL){
   //    printf("Couldnt open file.\n");
   //    exit(-1);
   // }

   // //read and deduplicate the query words and create the entry list
   // entry_list query_entry_list = read_queries(&number,fp);

   // printf("\n\nlist of queries wards: \n");
   // print_list(query_entry_list);

   // //create BK tree from entry list
   // bk_index ix = NULL;
   // enum error_code code = build_entry_index(&query_entry_list,my_match_type,&ix);

   // if(code == NULL_POINTER){
   //    printf("Null Pointer Given\n");
   //    exit(-1);
   // }
   // //print_bk_tree(ix,0);

   // int number_of_documents = count_documents(fp) + 1;
   // //printf("\n\n%d documents to be read\n\n",number_of_documents);

   // //reset the pointer to the start of the file
   // rewind(fp);

   // //read and deduplicate the document words and create an entry list
   // int doc_number = 0;
   // entry_list* document_entry_list_array = read_documents(&doc_number,fp,number_of_documents);

   // //print entry list for each document
   // for(int i = 0 ; i < number_of_documents; i++){
   //    printf("\n\nthis is document %d\n",i);
   //    print_list(document_entry_list_array[i]);
   // }

   // printf("\n\n Results: \n\n");

   // //check every entry list in BK
   // for(int i = 0 ; i < number_of_documents; i++){
   //    printf("Checking document no. %d:\n\n",i+1);
   //    check_entry_list(document_entry_list_array[i],&ix,threshold);
   //    printf("\n\n");
   // }
   
   // //destroy entry list for each document
   // for(int i = 0 ; i < number_of_documents; i++){
   //    //printf("\nfreeing list %d\n",i);
   //    destroy_entry_list(&document_entry_list_array[i]);
   // }
   // //free the array
   // free(document_entry_list_array);

   // //destroy the BK entry list
   // destroy_entry_list(&query_entry_list);

   // //destroy the BK tree
   // destroy_entry_index(&ix);

   // fclose(fp);

   // printf("\n\n");
   return 0;
}

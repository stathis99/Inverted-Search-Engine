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

   FILE* fp = fopen("./files/queries.txt","r");
   if(fp == NULL){
      printf("Couldnt open file.\n");
      exit(-1);
   }

 //read and store all queries
    char ch;
	unsigned int query_id;
    int fres;
   

    //Edit distance structures
    
    // bk_index ix = NULL;
    // Hash_table** hash_tables_edit = malloc(sizeof(Hash_table*)* 29);
    // for(int i = 0; i <=28 ; i++){
    //    hash_tables_edit[i] = NULL;
    // } 

    //Humming distance structures
    bk_index* humming_root_table = malloc(sizeof(bk_index)*29);
    for(int i = 0; i <=28 ; i++){
       humming_root_table[i] = NULL;
    }  
    Hash_table** hash_tables_humming = malloc(sizeof(Hash_table*)* 29);
    for(int i = 0; i <=28 ; i++){
       hash_tables_humming[i] = NULL;
    } 

    //Exact matching structures

    Hash_table_exact** hash_tables_exact = malloc(sizeof(Hash_table_exact*)* 29);
    for(int i = 0; i <=28 ; i++){
       hash_tables_exact[i] = NULL;
    } 

    int bloom_filter[100];
    for(int i = 0; i<99; i++){
        bloom_filter[i] = 0;
    }

    //start processing queries
    while(1){
        fres = fscanf(fp, "%c %u ", &ch, &query_id);
        if(fres == EOF){
            break;
        }
		if(ch == 's'){
            //printf("Reading query %d\n",query_id);
            int match_type;
			int match_dist;

			if(EOF == fscanf(fp, "%d %d %*d %[^\n\r] ", &match_type, &match_dist, temp)){
				printf("Corrupted Test File at Read Queries.\n");
				exit(-1);
			}

            //process files with match_type == 2
            if(match_type == 2){
                //deduplicate query words, insert them into the list
                //deduplicate_edit_distance(temp, query_id, match_dist, match_type, hash_tables_edit, &ix);
            }else if(match_type == 0){
                deduplicate_exact_matching(temp, query_id, match_dist, match_type, hash_tables_exact, bloom_filter);
            }else if(match_type == 1){
                //deduplicate_humming(temp, query_id, match_dist, match_type, hash_tables_humming, humming_root_table);
            }
        }else{
            break;
        }
		
    }

    //printing of each struct follows
    //print_hash_table_exact(hash_tables_exact);

    //print_hash_tables(hash_tables_edit);
    //print_bk_tree(ix,0);

    print_hash_tables(hash_tables_humming);
    for(int i=0; i<=28;i++){
        print_bk_tree(humming_root_table[i],0);
    }

    //Edit distance structures free'd
    /*delete_hash_tables_edit(hash_tables_edit);
    destroy_entry_index(&ix);*/

    //Humming distance structures free'd
    //delete_hash_tables_humming(hash_tables_humming,humming_root_table);

    //Exact match structure free'd
    delete_hash_tables_exact(hash_tables_exact);

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
   // //choose EDIT_DIST or HUMMING_DIST
   // if(atoi(argv[1]) == 1){
   //    my_match_type = EDIT_DIST;
   // }else if(atoi(argv[1]) == 2){
   //    my_match_type = HUMMING_DIST;
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

#include <stdio.h>
#include <stdlib.h>
#include "./include/structs.h"
#include <string.h>

int main(int argc, char* argv[]){

   if(argc != 3){
      printf("Invalid number of arguments given\n");
      exit(-1);
   }
   enum match_type my_match_type;
   //choose EDIT_DIST or HUMMING_DIST
   if(atoi(argv[1]) == 1){
      my_match_type = EDIT_DIST;
   }else if(atoi(argv[1]) == 2){
      my_match_type = HUMMING_DIST;
   }else{
      printf("Invalid match type given\n");
      exit(-1);
   }
   //chose a threshold 
   int threshold = atoi(argv[2]);

   int number = 0;

   //open the file we are reading from
   FILE* fp = fopen("./files/queries.txt","r");
   if(fp == NULL){
      printf("Couldnt open file.\n");
      exit(-1);
   }

   //read and deduplicate the query words and create the entry list
   entry_list query_entry_list = read_queries(&number,fp);

   printf("\n\nlist of queries wards: \n");
   print_list(query_entry_list);

   //create BK tree from entry list
   bk_index ix = NULL;
   //enum error_code code = build_entry_index(&query_entry_list,my_match_type,&ix);
   enum error_code code = build_entry_index_no_sort(&query_entry_list,my_match_type,&ix);

   if(code == NULL_POINTER){
      printf("Null Pointer Given\n");
      exit(-1);
   }
   print_bk_tree(ix,0);

   int number_of_documents = count_documents(fp) + 1;
   //printf("\n\n%d documents to be read\n\n",number_of_documents);

   //reset the pointer to the start of the file
   rewind(fp);

   //read and deduplicate the document words and create an entry list
   int doc_number = 0;
   entry_list* document_entry_list_array = read_documents(&doc_number,fp,number_of_documents);

   //print entry list for each document
   for(int i = 0 ; i < number_of_documents; i++){
      printf("\n\nthis is document %d\n",i);
      print_list(document_entry_list_array[i]);
   }

   //check every entry list in BK
   for(int i = 0 ; i < number_of_documents; i++){
      printf("Checking document no. %d:\n",i+1);
      check_entry_list(document_entry_list_array[i],&ix,threshold);
   }
   
   //destroy entry list for each document
   for(int i = 0 ; i < number_of_documents; i++){
      //printf("\nfreeing list %d\n",i);
      destroy_entry_list(&document_entry_list_array[i]);
   }
   //free the array
   free(document_entry_list_array);

   //destroy the BK entry list
   destroy_entry_list(&query_entry_list);

   //destroy the BK tree
   destroy_entry_index(&ix);

   fclose(fp);

   printf("\n\n");
   return 0;
}

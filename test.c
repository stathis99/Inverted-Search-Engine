#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include <string.h>

int main(int argc, char* argv[]){
   printf("Hello, World!\n");
   FILE* fp;
   //fp = fopen(argv[1],"r");
   fp = fopen("queries.txt","r");
   //create word 
   char key_word[] = "str1";
   word* my_word = (word*)malloc(sizeof(word));
   my_word->key_word = (char*)malloc(strlen(key_word)*sizeof(key_word));
   strcpy(my_word->key_word,key_word);

   //create instance of entry to be initialized
   //entry my_entry = NULL;
   //my_entry = malloc(sizeof(entry));
   //create_entry(my_word,&my_entry);
   entry my_entry = NULL;
   create_entry(my_word,&my_entry);

   entry_list my_entry_list = NULL;
   create_entry_list(&my_entry_list); 

   get_number_entries(&my_entry_list);
   add_entry(&my_entry_list,&my_entry);
   printf("%s\n",my_entry_list->entry_node->this_word->key_word);
   //initialize payload
  // my_entry->payload = (char**)malloc(2*sizeof(char*));
   /*char payload1[] = "str2";
   my_entry->payload[0] = (char*)malloc(strlen(payload1)*sizeof(payload1));
   strcpy(my_entry->payload[0],payload1);
   char payload2[] = "str3";
   my_entry->payload[1] = (char*)malloc(strlen(payload2)*sizeof(payload2));
   strcpy(my_entry->payload[1],payload2);*/

   //check what we did
   //printf("e->this_word->key_word:%s\n",my_entry->this_word->key_word);
   /*printf("%s\n",my_entry->payload[1]);
   printf("%s\n",my_word->key_word);
   printf("%s\n",my_entry->this_word->key_word);*/

   //free allocated memory
   /*free(my_word->key_word);
   free(my_word);
   free(my_entry->payload[0]);
   free(my_entry->payload[1]);
   free(my_entry->payload);
   free(my_entry);*/
   /*Entry* my_entry = create_entry();
   if(my_entry == NULL){
      printf("Not initialized\n");
   }else{
      printf("Initialized\n");
      printf("%s\n",my_entry->word);
      free(my_entry->word);
      free(my_entry);
   }*/
   //destroy_entry(&my_entry);
   destroy_entry_list(&my_entry_list);
   fclose(fp);
   return 0;
}
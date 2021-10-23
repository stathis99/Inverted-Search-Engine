#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include <string.h>

int main(int argc, char* argv[]){


//<---------create 3 words for testing----------------------------------------

   //create word 1
   char key_word1[] = "str1";
   word* my_word1 = (word*)malloc(sizeof(word));
   my_word1->key_word = (char*)malloc(strlen(key_word1)*sizeof(key_word1));
   strcpy(my_word1->key_word,key_word1);

   //create word 2
   char key_word2[] = "str2";
   word* my_word2 = (word*)malloc(sizeof(word));
   my_word2->key_word = (char*)malloc(strlen(key_word2)*sizeof(key_word2));
   strcpy(my_word2->key_word,key_word2);

   //create word 3
   char key_word3[] = "str1";
   word* my_word3 = (word*)malloc(sizeof(word));
   my_word3->key_word = (char*)malloc(strlen(key_word3)*sizeof(key_word3));
   strcpy(my_word3->key_word,key_word3);

   //create word 4
   char key_word4[] = "str4";
   word* my_word4 = (word*)malloc(sizeof(word));
   my_word4->key_word = (char*)malloc(strlen(key_word4)*sizeof(key_word4));
   strcpy(my_word4->key_word,key_word4);


//---------------create a list to store entries------------------------------

   entry_list my_entry_list = NULL;
   create_entry_list(&my_entry_list); 


//---------------create and add entries to to the list-------------------------

   entry my_entry = NULL;

   create_entry(my_word1,&my_entry);
   add_entry(&my_entry_list,&my_entry);

   create_entry(my_word2,&my_entry);
   add_entry(&my_entry_list,&my_entry);

   create_entry(my_word3,&my_entry);
   add_entry(&my_entry_list,&my_entry);

   create_entry(my_word4,&my_entry);
   add_entry(&my_entry_list,&my_entry);




   //print entire list
   print_list(my_entry_list);

   //print first entry
   entry* first_entry = get_first(&my_entry_list);
   printf("\n first entry:  %s\n ", (*first_entry)->this_word->key_word);





   //create instance of entry to be initialized
   //entry my_entry = NULL;
   //my_entry = malloc(sizeof(entry));
   //create_entry(my_word,&my_entry);
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
   return 0;
}
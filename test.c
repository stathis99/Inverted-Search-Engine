#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include <string.h>

int main(int argc, char* argv[]){


//<---------create 3 words for testing----------------------------------------

   //create word 1
   char key_word1[] = "hell";
   word* my_word1 = (word*)malloc(sizeof(word));
   my_word1->key_word = (char*)malloc(strlen(key_word1)*sizeof(key_word1));
   strcpy(my_word1->key_word,key_word1);

   //create word 2
   char key_word2[] = "help";
   word* my_word2 = (word*)malloc(sizeof(word));
   my_word2->key_word = (char*)malloc(strlen(key_word2)*sizeof(key_word2));
   strcpy(my_word2->key_word,key_word2);

   //create word 3
   char key_word3[] = "fall";
   word* my_word3 = (word*)malloc(sizeof(word));
   my_word3->key_word = (char*)malloc(strlen(key_word3)*sizeof(key_word3));
   strcpy(my_word3->key_word,key_word3);

   //create word 4
   /*char key_word4[] = "felt";
   word* my_word4 = (word*)malloc(sizeof(word));
   my_word4->key_word = (char*)malloc(strlen(key_word4)*sizeof(key_word4));
   strcpy(my_word4->key_word,key_word4);

   //create word 5
   char key_word5[] = "fell";
   word* my_word5 = (word*)malloc(sizeof(word));
   my_word5->key_word = (char*)malloc(strlen(key_word5)*sizeof(key_word5));
   strcpy(my_word5->key_word,key_word5);

   //create word 6
   char key_word6[] = "small";
   word* my_word6 = (word*)malloc(sizeof(word));
   my_word6->key_word = (char*)malloc(strlen(key_word6)*sizeof(key_word6));
   strcpy(my_word6->key_word,key_word6);

   //create word 7
   char key_word7[] = "melt";
   word* my_word7 = (word*)malloc(sizeof(word));
   my_word7->key_word = (char*)malloc(strlen(key_word7)*sizeof(key_word7));
   strcpy(my_word7->key_word,key_word7);*/


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

   /*create_entry(my_word4,&my_entry);
   add_entry(&my_entry_list,&my_entry);

   create_entry(my_word5,&my_entry);
   add_entry(&my_entry_list,&my_entry);

   create_entry(my_word6,&my_entry);
   add_entry(&my_entry_list,&my_entry);

   create_entry(my_word7,&my_entry);
   add_entry(&my_entry_list,&my_entry);


   //print entire list
   print_list(my_entry_list);

   //print first entry
   entry* first_entry = get_first(&my_entry_list);
   printf("\n first entry:  %s\n ", (*first_entry)->this_word->key_word);
   
   //print next entry
   entry* next_entry = get_next(&my_entry_list);
   printf("%s\n",(*next_entry)->this_word->key_word);

   //print next next entry
   entry* next_next_entry = get_next(&my_entry_list->next);
   printf("%s\n",(*next_next_entry)->this_word->key_word);

   printf("\n distance is : %d   ", editDist(my_word1->key_word,my_word6->key_word,strlen(my_word1->key_word),strlen(my_word6->key_word)));

*/
   entry* first = get_first(&my_entry_list);
   printf("first:%s\n",(*first)->this_word->key_word);
   entry* next = get_next(&my_entry_list,first);
   printf("first:%s\n",(*next)->this_word->key_word);
   print_list(my_entry_list);
   destroy_entry_list(&my_entry_list);

   /*char* str1 = "melted";
   char* str2 = "hell";
   humming_distance(str1,str2,strlen(str1),strlen(str2));*/
   return 0;
}
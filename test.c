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
   char key_word2[] = "fall";
   word* my_word2 = (word*)malloc(sizeof(word));
   my_word2->key_word = (char*)malloc(strlen(key_word2)*sizeof(key_word2));
   strcpy(my_word2->key_word,key_word2);

   //create word 3
   char key_word3[] = "help";
   word* my_word3 = (word*)malloc(sizeof(word));
   my_word3->key_word = (char*)malloc(strlen(key_word3)*sizeof(key_word3));
   strcpy(my_word3->key_word,key_word3);

   //create word 4
   char key_word4[] = "felt";
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
   strcpy(my_word7->key_word,key_word7);

   //create word 8
   char key_word8[] = "hellk";
   word* my_word8 = (word*)malloc(sizeof(word));
   my_word8->key_word = (char*)malloc(strlen(key_word8)*sizeof(key_word8));
   strcpy(my_word8->key_word,key_word8);

   //create word 10
   char key_word10[] = "call";
   word* my_word10 = (word*)malloc(sizeof(word));
   my_word10->key_word = (char*)malloc(strlen(key_word10)*sizeof(key_word10));
   strcpy(my_word10->key_word,key_word10);

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

   create_entry(my_word5,&my_entry);
   add_entry(&my_entry_list,&my_entry);

   create_entry(my_word6,&my_entry);
   add_entry(&my_entry_list,&my_entry);

   create_entry(my_word7,&my_entry);
   add_entry(&my_entry_list,&my_entry);

   create_entry(my_word8,&my_entry);
   add_entry(&my_entry_list,&my_entry);

   create_entry(my_word10,&my_entry);
   add_entry(&my_entry_list,&my_entry);
   //print entire list
   print_list(my_entry_list);


   bk_index ix = NULL;
   build_entry_index(&my_entry_list,EDIT_DIST,&ix);
   print_bk_tree(ix,0);

   //create a word to search in tree
   char key_word9[] = "henn";
   word* my_word9 = (word*)malloc(sizeof(word));
   my_word9->key_word = (char*)malloc(strlen(key_word9)*sizeof(key_word9));
   strcpy(my_word9->key_word,key_word9);
   //create list for results
   //entry_list results = NULL;
   //create_entry_list(&results); 

   //lookup_entry_index(my_word9, &ix, 1, &results);

   //print_list(results);



   //print_bk_tree(ix,0);
   ///printf("%d Nodes in List\n",get_number_entries(&my_entry_list));
   //destroy_entry_list(&my_entry_list);
   /*char* str1 = "melted";
   char* str2 = "hell";
   humming_distance(str1,str2,strlen(str1),strlen(str2));*/
   //int number = 0;
   //char** array = read_document(&number);

   //free array
   // for(int i=0; i < number;i++){
   //    free(array[i]);
   // }
   // free(array);

   destroy_entry_index(&ix);

   ix = NULL;
   build_entry_index(&my_entry_list,HUMMING_DIST,&ix);
   print_bk_tree(ix,0);

   destroy_entry_list(&my_entry_list);
   destroy_entry_index(&ix);
   free(my_word9->key_word);
   free(my_word9);
   printf("\n\n");
   return 0;
}
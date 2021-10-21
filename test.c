#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include <string.h>

int main() {
   printf("Hello, World!\n");

   //create word 
   char key_word[] = "str1";
   word* my_word = (word*)malloc(sizeof(word));
   my_word->keyword = (char*)malloc(strlen(key_word)*sizeof(key_word));
   strcpy(my_word->keyword,key_word);

   //create instance of entry to be initialized
   entry* my_entry = NULL;
   create_entry(my_word,my_entry);

   //free allocated memory
   free(my_word->keyword);
   free(my_word);
   free(my_entry);
   /*Entry* my_entry = create_entry();
   if(my_entry == NULL){
      printf("Not initialized\n");
   }else{
      printf("Initialized\n");
      printf("%s\n",my_entry->word);
      free(my_entry->word);
      free(my_entry);
   }*/
   return 0;
}
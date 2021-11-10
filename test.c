#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include <string.h>

int main(int argc, char* argv[]){

   int number = 0;

   entry_list my_entry_list = read_document(&number);
   printf("Number in EntryList was: %d\n",number);
   print_list(my_entry_list);
   bk_index ix = NULL;
   build_entry_index(&my_entry_list,EDIT_DIST,&ix);
   number = 0;
   print_bk_tree(ix,0,&number);
   printf("Number in BK tree now is: %d\n",number);
   destroy_entry_index(&ix);
   destroy_entry_list(&my_entry_list);
   printf("\n\n");
   return 0;
}
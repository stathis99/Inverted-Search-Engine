#include "structs.h"
#include <stdlib.h>
#include <string.h>

//can this even work?
enum error_code create_entry1(const word* w, entry* e){
    printf("gets\n");
    //printf("%s",w->key_word);
    e = (entry*)malloc(sizeof(entry)); //this line causes segmentation
    e->this_word = (word*)malloc(sizeof(word));
    e->this_word->key_word = (char*)malloc(sizeof(char*)*strlen(w->key_word));
    strcpy(e->this_word->key_word,w->key_word);
    //e->this_word = (word*)w;
    enum error_code my_enum = SUCCESS;
    return my_enum;
}

enum error_code create_entry(const word* w, entry* e){
    //e->this_word = (word*)malloc(sizeof(word));
    /*e->this_word->key_word = (char*)malloc(sizeof(char*)*strlen(w->key_word));
    strcpy(e->this_word->key_word,w->key_word);*/
    
    e->this_word = (word*)w;
    //printf("e->this_word->key_word:%s\n",e->this_word->key_word);
    enum error_code my_enum = SUCCESS;
    return my_enum;
}

/*Entry* create_entry(){
    Entry* my_entry = (Entry*)malloc(sizeof(Entry));
    char string1[] = "str1";
    my_entry->word = (char*)malloc(strlen(string1)*sizeof(string1));
    strcpy(my_entry->word,string1);
    char string2[] = "str12";
    char string3[] = "str13";
    char** array = malloc (sizeof(char*));
    array[]

    /*char[] string2 = "str2";
    char* string3 = malloc(sizeof);
    my_entry->word = NULL;
    my_entry->payload = NULL;
    return my_entry;
}*/
#include "structs.h"
#include <stdlib.h>
#include <string.h>

enum error_code create_entry(const word* w, entry* e){
    printf("gets\n");
    printf("%s",w->keyword);
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
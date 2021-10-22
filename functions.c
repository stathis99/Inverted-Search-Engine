#include "structs.h"
#include <stdlib.h>
#include <string.h>

enum error_code create_entry(const word* w, entry* e){
    //e->this_word = (word*)malloc(sizeof(word));
    /*e->this_word->key_word = (char*)malloc(sizeof(char*)*strlen(w->key_word));
    strcpy(e->this_word->key_word,w->key_word);*/
    //*e = malloc(sizeof(entry));
    *e = malloc(sizeof(Entry));
    (*e)->this_word = (word*)w;
    (*e)->payload = malloc(sizeof(char*)*2);
    char payload1[] = "str2";
    (*e)->payload[0] = malloc(sizeof(payload1)*strlen(payload1));
    strcpy((*e)->payload[0],payload1);
    //printf("e->this_word->key_word:%s\n",e->this_word->key_word);
    enum error_code my_enum = SUCCESS;
    return my_enum;
}

enum error_code destroy_entry(entry* e){
    free((*e)->payload[0]);
    free((*e)->payload);
    free((*e)->this_word->key_word);
    free((*e)->this_word);
    free((*e));
}
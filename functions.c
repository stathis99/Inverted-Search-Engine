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

enum error_code create_entry_list(entry_list* el){
    *el = (entry_list)malloc(sizeof(Entry_List));
    (*el)->next = NULL;
    (*el)->entry_node = NULL;
}

enum error_code destroy_entry_list(entry_list* el){
    entry_list temp ;
    while(*el != NULL){
        temp = *el;
        *el = (*el)->next;
        free(temp->entry_node);
        free(temp);
    }
}

unsigned int get_number_entries(const entry_list* el){
    entry_list temp = *el;
    int number = 0;
    while(temp != NULL){
        number++;
        temp = temp->next;
    }
    return number;
}

enum error_code add_entry(entry_list* el, const entry* e){
    entry_list temp = *el;

    //create the new node
    entry_list new_node = (entry_list)malloc(sizeof(Entry_List));
    new_node->next = NULL;
    new_node->entry_node = (entry)e;

    while( temp->next != NULL){
        temp = temp->next;
    }

    //add new node to the end
    temp = new_node;
}
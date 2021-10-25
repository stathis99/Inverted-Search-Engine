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
    
    enum error_code my_enum = SUCCESS;
    return my_enum;
}

enum error_code create_entry_list(entry_list* el){
    *el = malloc(sizeof(Entry_List));
    (*el)->next = NULL;
    (*el)->entry_node = NULL;

    enum error_code my_enum = SUCCESS;
    return my_enum;
}

enum error_code destroy_entry_list(entry_list* el){
    entry_list temp;
    while(*el != NULL){
        temp = *el;
        *el = (*el)->next;
        //free(temp->entry_node);
        destroy_entry(&(temp->entry_node));
        free(temp);
    }

    enum error_code my_enum = SUCCESS;
    return my_enum;
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
    if((*el)->entry_node == NULL){      //first node, initialize it
        (*el)->entry_node = (entry)(*e);
        enum error_code my_enum = SUCCESS;
        return my_enum;
    }else{
        entry_list new_node = malloc(sizeof(Entry_List));
        new_node->next = NULL;
        new_node->entry_node = (entry)(*e);

        entry_list temp = *el;
        while(temp->next != NULL){
            temp = temp->next;
        }
        temp->next = new_node;
        enum error_code my_enum = SUCCESS;
        return my_enum;
    }
}


void print_list(entry_list el){
    printf("number of entries: %d \n", get_number_entries(&el));
    while(el != NULL){
        printf("%s -> ",el->entry_node->this_word->key_word);
        el = el-> next;
    }
}


entry* get_first(const entry_list* el){ 
    return &((*el)->entry_node);
}

 entry* get_next(const entry_list* el){
     entry* next_node = &((*el)->next->entry_node);

     return next_node;
 }


// enum error_code build_entry_index(const entry_list* el, enum match_type type, index* ix){

// }
int min2(int x, int y){ return x > y ? y:x;}

int min3(int x, int y, int z) { return min2(min2(x, y),z); }

int editDist(char* str1, char* str2, int m, int n)
{
    // If first string is empty, the only option is to
    // insert all characters of second string into first
    if (m == 0)
        return n;
  
    // If second string is empty, the only option is to
    // remove all characters of first string
    if (n == 0)
        return m;
  
    // If last characters of two strings are same, nothing
    // much to do. Ignore last characters and get count for
    // remaining strings.
    if (str1[m - 1] == str2[n - 1])
        return editDist(str1, str2, m - 1, n - 1);
  
    // If last characters are not same, consider all three
    // operations on last character of first string,
    // recursively compute minimum cost for all three
    // operations and take minimum of three values.
    return 1
           + min3(editDist(str1, str2, m, n - 1), // Insert
                 editDist(str1, str2, m - 1, n), // Remove
                 editDist(str1, str2, m - 1,
                          n - 1) // Replace
             );
}
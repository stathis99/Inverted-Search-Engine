#include "structs.h"
#include <stdlib.h>
#include <string.h>

enum error_code create_entry(const word* w, entry* e){
    *e = malloc(sizeof(Entry));
    (*e)->this_word = (word*)w;
    (*e)->payload = malloc(sizeof(char*)*2);
    char payload1[] = "str2";
    (*e)->payload[0] = malloc(sizeof(payload1)*strlen(payload1));
    strcpy((*e)->payload[0],payload1);
    (*e)->next = NULL;
    return SUCCESS;
}

enum error_code destroy_entry(entry* e){
    free((*e)->payload[0]);
    free((*e)->payload);
    free((*e)->this_word->key_word);
    free((*e)->this_word);
    //free((*e));
    
    enum error_code my_enum = SUCCESS;
    return my_enum;
}

enum error_code create_entry_list(entry_list* el){
    *el = malloc(sizeof(Entry_List));
    if(el == NULL){
        return ERROR;
    }
    (*el)->first_node = NULL;
    (*el)->last_node = NULL;
    (*el)->node_count = 0;
    enum error_code my_enum = SUCCESS;
    return my_enum;
}

enum error_code destroy_entry_list(entry_list* el){
    if(*el == NULL){        //it is empty
        return SUCCESS;
    }
    entry temp;
    while((*el)->first_node != NULL){
        temp = (*el)->first_node;
        (*el)->first_node = (*el)->first_node->next;
        destroy_entry(&temp);
        free(temp);
    }
    free(*el);
    /*entry_list temp;
    while(*el != NULL){
        temp = *el;
        *el = (*el)->next;
        //free(temp->entry_node);
        destroy_entry(&(temp->entry_node));
        free(temp);
    }*/
    return SUCCESS;
}

/*unsigned int get_number_entries(const entry_list* el){
    entry_list temp = *el;
    int number = 0;
    while(temp != NULL){
        number++;
        temp = temp->next;
    }
    return number;
}

*/
enum error_code add_entry(entry_list* el, const entry* e){
    if((*el)->first_node == NULL){
        (*el)->node_count++;
        (*el)->first_node = (entry)(*e);
    }else if((*el)->last_node == NULL){
        (*el)->first_node->next = *e;
        (*el)->node_count++;
        (*el)->last_node = (entry)(*e);
    }else{
        (*el)->last_node->next = *e;
        (*el)->node_count++;
        (*el)->last_node = (entry)(*e);
    }
    return SUCCESS;
}


void print_list(const entry_list el){
    entry head = el->first_node;
    while(head != NULL){
        printf("%s -> ",head->this_word->key_word);
        head = head-> next;
    }
}


entry* get_first(const entry_list* el){ 
    return &((*el)->first_node);
}

entry* get_next(const entry_list* el, entry* e){
    //entry* next_node = &((*el)->next->entry_node);

    return &((*e)->next);
}

/*
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

int humming_distance(char* str1, char* str2, int m,int n){
    //if one word has more letters add difference to distance
    int distance = m > n ? (m-n) : (n-m);
    
    //compare characters one by one
    //compare up to last common index
    int last_character = m > n ? (m-distance) : (n-distance);
    
    for(int i=0; i < last_character; i++){
        printf("%c == %c\n",str1[i],str2[i]);
        if(str1[i] != str2[i]){
            distance++;
        }
    }
    printf("distance=%d\n",distance);
    return distance;
}*/
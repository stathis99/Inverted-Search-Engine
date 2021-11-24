#include "declarations.h"
#include <stdlib.h>
#include <string.h>

//entry functions
enum error_code create_entry(const word* w, entry* e){
    *e = malloc(sizeof(Entry));

    //duplicate the word
    (*e)->this_word = (word*)w;

    (*e)->next = NULL;
    return SUCCESS;
}

enum error_code destroy_entry(entry* e){
    if(*e == NULL){
        return NULL_POINTER;
    }

    free(*((*e)->this_word));
    free(*e);
    return SUCCESS;
}

//entry list functions
enum error_code create_entry_list(entry_list* el){
    *el = malloc(sizeof(Entry_List));
    if(el == NULL){
        return NULL_POINTER;
    }
    (*el)->first_node = NULL;
    (*el)->last_node = NULL;
    (*el)->node_count = 0;

    return SUCCESS;
}

enum error_code destroy_entry_list(entry_list* el){
    if(*el == NULL){        //it is empty
        return NULL_POINTER;
    }

    //free every entry node
    entry temp;
    while((*el)->first_node != NULL){
        temp = (*el)->first_node;
        (*el)->first_node = (*el)->first_node->next;
        destroy_entry(&temp);
    }

    //free the list itself
    free(*el);
    *el = NULL;
    return SUCCESS;
}

enum error_code add_entry(entry_list* el, const entry* e){
    if(*e == NULL){
        return NULL_POINTER;
    }
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
    if(el == NULL){
        return;
    }
    entry head = el->first_node;
    while(head != NULL){
        printf("%s\n",*head->this_word);
        head = head->next;
    }
}

entry* get_first(const entry_list* el){
    if(*el == NULL){
        return NULL;
    } 
    return &((*el)->first_node);
}

entry* get_next(const entry_list* el, entry* e){
    if(*el == NULL){
        return NULL;
    }
    return &((*e)->next);
}

unsigned int get_number_entries(const entry_list* el){
    if(*el == NULL){
        return 0;
    }
    return (*el)->node_count;
}

//min functions
int min_of_2(int x, int y){ 
    return x > y ? y:x;
}

int min_of_3(int x, int y, int z) { 
    return min_of_2( min_of_2(x,y), z); 
}

//distance functions
int humming_distance(char* str1, char* str2, int len){              //this is called ONLY IF words have equal length, caller checks that
    int distance = 0;
    
    //compare characters one by one
    for(int i=0; i < len ; i++){
        if(str1[i] != str2[i]){
            distance++;
        }
    }
    return distance;
}

int edit_dist(char* str1, char* str2, int len1, int len2){
    //Create a table to store cost of current subproblem
    int cost[len1 + 1][len2 + 1];
  
    //Fill cost[][] in bottom up manner
    for (int i = 0; i <= len1; i++) {
        for (int j = 0; j <= len2; j++) {
            //If current substring of str1 is empty, 
            //cost is increasing by 1 for every letter (insert operation)
            if (i == 0)
                cost[i][j] = j; // Min. operations = j
  
            //If current substring of str2 is empty, 
            //cost is increasing by 1 for every letter (insert operation)
            else if (j == 0)
                cost[i][j] = i; // Min. operations = i
  
            //Last characters are the same, no added cost,
            //so take cost of previous substring
            else if (str1[i - 1] == str2[j - 1])
                cost[i][j] = cost[i - 1][j - 1];
  
            //Last characters are not the same, 
            //so calculate minimum cost of the 3 available operations and add 1
            else
                cost[i][j]
                    = 1
                      + min_of_3(cost[i][j - 1], // Insert
                            cost[i - 1][j], // Remove
                            cost[i - 1][j - 1]); // Replace
        }
    }
  
    return cost[len1][len2];
}

//global query list to be used
query_list my_query_list = NULL;
query head = NULL;
query tail = NULL;

ErrorCode InitializeIndex(){
    //initialize our query_list;
    my_query_list = malloc(sizeof(Query_List));
    return EC_SUCCESS;
}

ErrorCode DestroyIndex(){
    //free our query_list
    destroy_query_list();
    return EC_SUCCESS;
}

ErrorCode StartQuery(Query_id query_id, const char* query_str, MatchType match_type, unsigned int match_dist){
	if(head == NULL){
        //allocate node
        head = malloc(sizeof(Query));

        //initialize data
        head->query_id = query_id;
        strcpy(head->str,query_str);
        head->match_dist = match_dist;
        head->match_type = match_type;

        //insert it to the list
        head->next = NULL;
        my_query_list->head = head;
    }else if(tail == NULL){
        //allocate node
        tail = malloc(sizeof(Query));

        //initialize data
        tail->query_id = query_id;
        strcpy(tail->str,query_str);
        tail->match_dist = match_dist;
        tail->match_type = match_type;

        //insert it to the list
        tail->next = NULL;
        head->next = tail; 
    }else{
        //allocate node
        query temp = tail;
        tail->next = malloc(sizeof(Query));

        //initialize data
        tail->next->query_id = query_id;
        strcpy(tail->next->str,query_str);
        tail->next->match_dist = match_dist;
        tail->next->match_type = match_type;
        
        //insert it to the list
        tail->next->next = NULL;
        tail = tail->next; 
    }
}

enum error_code destroy_query_list(){
    if (my_query_list == NULL){
        return NULL_POINTER;
    }

    query temp;
    while(my_query_list->head != NULL){
        temp = my_query_list->head;
        my_query_list->head = my_query_list->head->next;
        free(temp);
    }

    //free the list itself
    free(my_query_list);
    my_query_list = NULL;
    return SUCCESS;
}

enum error_code add_query(query my_query){
    //first node is head of the list
    if(my_query_list->head == NULL){
        my_query_list->head == my_query;
        return SUCCESS;
    }

    //add it after the last node
    query temp = my_query_list->head;
    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = my_query;
    return SUCCESS;
}

void print_query_list(){
    printf("Printing query list\n");
    query temp = my_query_list->head;
    while(temp != NULL){
        printf("Query: %d %d %d %s\n",temp->query_id,temp->match_type,
            temp->match_dist,temp->str);
        temp = temp->next;
    }
}

ErrorCode MatchDocument(Doc_ID doc_id, const char* doc_str){

}
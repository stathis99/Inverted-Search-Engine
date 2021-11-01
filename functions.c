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
    return SUCCESS;
}

unsigned int get_number_entries(const entry_list* el){
    entry temp = (*el)->first_node;
    int number = 0;
    while(temp != NULL){
        number++;
        temp = temp->next;
    }
    return (*el)->node_count;
}


// unsigned int get_number_entries(const entry_list* el){
//     entry_list temp = *el;
//     int number = 0;
//     while(temp != NULL){
//         number++;
//         temp = temp->next;
//     }
//     return number;
// }


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
        printf("%s - ",head->this_word->key_word);
        head = head-> next;
    }
}


entry* get_first(const entry_list* el){ 
    return &((*el)->first_node);
}

entry* get_next(const entry_list* el, entry* e){
    return &((*e)->next);
}


// enum error_code build_entry_index(const entry_list* el, enum match_type type, index* ix){


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
}

char** read_document(int* number){
    char** document_words = NULL;
    int number_of_words = 0;
    FILE* fp = fopen("doc1.txt","r");
    if(fp == NULL){
        return NULL;
    }

    char c;
    char* word = NULL;
    int word_length = 0;
    while((c = fgetc (fp)) != EOF){

        if((c == ' ' || c == '\n') && (word_length > 0)){     //full word has been read. add letters to form the word
            //store word
            //move to next
            word[word_length] = '\0';
            if(document_words == NULL){         //first word, initialize array
                document_words = malloc(sizeof(char*));
                document_words[0] = malloc(sizeof(char)*(word_length+1));
                strcpy(document_words[0],word);
                free(word);
                number_of_words++;
            }else{
                int exists = 0;
                printf("\nSearching for word: %s\n",word);
                //check that this word doesnt already exist in the array; deduplicate here
                for(int i=0; i < number_of_words;i++){
                    if(word != NULL && document_words[i] != NULL){
                       printf("Comparing: %s = %s\n",document_words[i],word);
                       if(strcmp(document_words[i],word)==0){
                           printf("\nmatch found for word: %s\n",word);
                           free(word);
                           exists = 1;
                           break;
                       }
                    }
                }
                if(exists == 0){
                    //if it doesnt, append the array
                    number_of_words++;
                    char** temp_document = realloc(document_words,(number_of_words)*sizeof(char*));
                    document_words = temp_document;
                    document_words[number_of_words-1] = malloc(sizeof(char)*(word_length+1));
                    strcpy(document_words[number_of_words-1],word);
                    free(word);
                }
            }
            word_length = 0;
            word = NULL;
        }else{
            word_length++;
            char* temp_word;
            if(word != NULL){
                temp_word = (char*)realloc(word,(word_length+1)*sizeof(char));
                if(temp_word == NULL){
                    free(word);
                    return NULL;
                }
                word = temp_word;
            }else{
                word = (char*)realloc(word,(word_length+1)*sizeof(char));
            }
            word[word_length-1] = c;
        }
    }
    fclose(fp);
    printf("Read %d words\n",number_of_words);
    for(int i=0; i < number_of_words; i++){
        printf("Index[%d]: %s\n",i,document_words[i]);
    }
    *number = number_of_words;
    return document_words;
}


 void print_bk_tree(bk_index ix, int pos){   
    struct children_list*  temp = ix->children_list;
    printf("\n\n   %s:%d  %d \n\n",ix->this_word->key_word,ix->weight,pos);

    while(temp != NULL){
        print_bk_tree(temp->child,pos-1);
        temp = temp->next;   
    }
    return;
 }

int distExists_ChildrenList(struct children_list* cl, int dist){

    while(cl !=  NULL){
        if(cl->weight == dist){
            return 1;
        }
        cl = cl->next;
    }
    return 0;
}

void bk_create_node(bk_index* ix,word* entry_word,int weight){

        (*ix) = malloc(sizeof(Index));
        (*ix)->weight = weight;
        (*ix)->this_word = (word*)malloc(sizeof(word));
        (*ix)->this_word = entry_word;
        (*ix)-> children_list = NULL;

}

void cl_create_node(struct children_list** nodes_children, word* entry_word,int dist){

    *nodes_children = malloc(sizeof(struct children_list));
    (*nodes_children)->weight = dist;
    (*nodes_children)->next = NULL;

    bk_create_node(&((*nodes_children)->child ),entry_word, dist);

}


int bk_add_node(bk_index* ix,word* entry_word){

    struct children_list* nodes_children = (*ix)->children_list;
    int dist = editDist(entry_word->key_word,(*ix)->this_word->key_word,strlen(entry_word->key_word),strlen((*ix)->this_word->key_word));

    //if node doesnt have children
    if(nodes_children == NULL){
        cl_create_node(&((*ix)->children_list),entry_word,dist);
        return 1; 
    }
    else{
        if(distExists_ChildrenList((*ix)-> children_list, dist) == 1){
            while(nodes_children->child->weight != dist){
                nodes_children = nodes_children->next;
            }
            bk_add_node(&(nodes_children->child),entry_word);
        }else{
            while(nodes_children->next != NULL){
            nodes_children = nodes_children -> next;
        }
        cl_create_node(&(nodes_children->next),entry_word,dist);
        return 1;
        }
    }
}



enum error_code build_entry_index(const entry_list* el, enum match_type type, bk_index* ix){
    entry temp_entry = (*el)->first_node;

    //create the root of the tree
    if((*ix) == NULL){
        bk_create_node(ix,temp_entry->this_word,0);
    }
    //for every word in the list add it to the tree
    temp_entry = temp_entry->next;
    while(temp_entry != NULL){
        bk_add_node(ix,temp_entry->this_word);
        temp_entry = temp_entry->next;
    }

    enum error_code my_enum = SUCCESS;
    return my_enum;
}
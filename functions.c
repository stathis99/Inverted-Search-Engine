#include "./include/structs.h"
#include <stdlib.h>
#include <string.h>


//entry functions
void free_word(word* my_word){
    free(my_word->key_word);
    free(my_word);
}


enum error_code create_entry(const word* w, entry* e){
    *e = malloc(sizeof(Entry));

    //duplicate the word
    (*e)->this_word = malloc(sizeof(word));
    (*e)->this_word->key_word = malloc(sizeof(char)*sizeof(w->key_word));
    strcpy((*e)->this_word->key_word,w->key_word);
    //instead of doing this, just copying the pointer
    //(*e)->this_word = (word*)w;
    (*e)->payload = NULL;
    (*e)->next = NULL;
    return SUCCESS;
}

enum error_code destroy_entry(entry* e){
    if(*e == NULL){
        return NULL_POINTER;
    }
    //free((*e)->payload[0]);
    //free((*e)->payload);
    free((*e)->this_word->key_word);
    free((*e)->this_word);

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
    entry temp;
    while((*el)->first_node != NULL){
        temp = (*el)->first_node;
        (*el)->first_node = (*el)->first_node->next;
        destroy_entry(&temp);
        free(temp);
    }
    free(*el);
    *el = NULL;
    return SUCCESS;
}

unsigned int get_number_entries(const entry_list* el){
    if(*el == NULL){
        return 0;
    }
    entry temp = (*el)->first_node;
    int number = 0;
    while(temp != NULL){
        number++;
        temp = temp->next;
    }
    return (*el)->node_count;
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


//min functions
int min2(int x, int y){ 
    return x > y ? y:x;
}

int min3(int x, int y, int z) { 
    return min2( min2(x,y), z); 
}


//distance functions
int editDist(char* str1, char* str2, int m, int n){
    // Create a table to store results of subproblems
    int dp[m + 1][n + 1];
  
    // Fill d[][] in bottom up manner
    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            // If first string is empty, only option is to
            // insert all characters of second string
            if (i == 0)
                dp[i][j] = j; // Min. operations = j
  
            // If second string is empty, only option is to
            // remove all characters of second string
            else if (j == 0)
                dp[i][j] = i; // Min. operations = i
  
            // If last characters are same, ignore last char
            // and recur for remaining string
            else if (str1[i - 1] == str2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
  
            // If the last character is different, consider
            // all possibilities and find the minimum
            else
                dp[i][j]
                    = 1
                      + min3(dp[i][j - 1], // Insert
                            dp[i - 1][j], // Remove
                            dp[i - 1][j - 1]); // Replace
        }
    }
  
    return dp[m][n];
}

int humming_distance(char* str1, char* str2, int m,int n){
    //if one word has more letters add difference to distance
    int distance = m > n ? (m-n) : (n-m);
    
    //compare characters one by one
    //compare up to last common index
    int last_character = m > n ? (m-distance) : (n-distance);
    
    for(int i=0; i < last_character; i++){
        if(str1[i] != str2[i]){
            distance++;
        }
    }
    return distance;
}


//bk functions
 void print_bk_tree(bk_index ix, int pos){   
    bk_index temp_child  = ix->child;
    
    printf("\n\n   %s:%d  %d \n\n",ix->this_word->key_word,ix->weight,pos);

    while(temp_child != NULL){
        print_bk_tree(temp_child,pos-1);
        temp_child = temp_child->next;   
    }
    return;
 }

//creates a bk tree node
void bk_create_node(bk_index* ix,word* entry_word,int weight){

        (*ix) = malloc(sizeof(Index));
        (*ix)->weight = weight;
        (*ix)->this_word = entry_word;
        (*ix)->next = NULL;
        (*ix)-> child = NULL;

}

int bk_add_node(bk_index* ix,word* entry_word,enum match_type type){

    bk_index temp_child  = (*ix)->child;
    bk_index previous_child = NULL;
    int dist;
    if(type == EDIT_DIST){
        dist = editDist(entry_word->key_word,(*ix)->this_word->key_word,strlen(entry_word->key_word),strlen((*ix)->this_word->key_word));
    }else{
        dist = humming_distance(entry_word->key_word,(*ix)->this_word->key_word,strlen(entry_word->key_word),strlen((*ix)->this_word->key_word));
    }
    //if ix doesnt have children create a node and set it as his  child
    if(temp_child == NULL){

        bk_create_node(&(*ix)->child, entry_word, dist);
        return 1; 

    }else{

        //for every child of ix 
        int child_distance = -1; 
        while(temp_child != NULL){

            //if exists child with same dist go to this child , child
            if(temp_child->weight == dist){
                bk_add_node(&temp_child, entry_word, type);
                return 1;
            }
            if(dist < temp_child->weight){
                if(temp_child == (*ix)->child){     //replaces previous child
                    bk_index new_index = NULL;
                    bk_create_node(&new_index,entry_word,dist);
                    (*ix)->child = new_index;
                    new_index->next = temp_child;
                    return 1;
                }else{                              //new node is between 2 existing ones, add it in the middle
                    bk_index new_index = NULL;
                    bk_create_node(&new_index,entry_word,dist);
                    bk_index temp_next = previous_child->next;
                    previous_child->next = new_index;
                    new_index->next = temp_child;
                    return 1;
                }
            }
            //if we wave seen all ix children and there is no child with same distance
            if(temp_child->next == NULL){
                //add new node at the end of children

                //modify here to add with increasing distance
                bk_create_node(&(temp_child->next),entry_word,dist);
                return 1;
            }
            previous_child = temp_child;
            temp_child = temp_child -> next;
        }
    }
    return 1;
}

enum error_code build_entry_index(const entry_list* el, enum match_type type, bk_index* ix){
    if((*el)->first_node == NULL){
        return NULL_POINTER;
    }
    entry temp_entry = (*el)->first_node;
    //create the root of the tree
    if((*ix) == NULL){
        bk_create_node(ix,temp_entry->this_word,0);
    }

    //for every word in the list add it to the tree
    temp_entry = temp_entry->next;
    while(temp_entry != NULL){
        bk_add_node(ix,temp_entry->this_word,type);
        temp_entry = temp_entry->next;
    }

    return SUCCESS;
}

enum error_code lookup_entry_index(const word* w, bk_index* ix, int threshold, entry_list* result){
    bk_index temp_child  = (*ix)->child;
    int dist = editDist(w->key_word,(*ix)->this_word->key_word,strlen(w->key_word),strlen((*ix)->this_word->key_word));
    int min_dist = dist - threshold;
    int max_dist = dist + threshold;

    //if ix keyword is close to w then create an entry and add it to the results
    if( dist <= threshold ){
        entry my_entry = NULL;
        create_entry((*ix)->this_word,&my_entry);
        add_entry(result,&my_entry);
    }

    //traverse the tree retrospectively 
    while(temp_child != NULL){
        //if distance child from the parent is  [𝑑 − 𝑛, 𝑑 + 𝑛] then call retrospectively 
        if(temp_child->weight >= min_dist && temp_child->weight <= max_dist ){
            lookup_entry_index(w,&temp_child,threshold,result);
        }
        temp_child = temp_child->next;   

        //=================================================================>edw prepei na ekmetaleftoume oti oi komvoi einai sthn seira 
    }
    
    return SUCCESS;
}

enum error_code destroy_entry_index(bk_index* ix){
    if(*ix == NULL){
        return NULL_POINTER;
    }

    bk_index temp_child  = (*ix)->child;
    free(*ix);
    bk_index temp_child_next;
    while(temp_child != NULL){
        temp_child_next = temp_child->next;
        destroy_entry_index(&temp_child);
        temp_child = temp_child_next;   
    }
    *ix = NULL;
    return SUCCESS;

}

entry_list read_document(int* number){
    int number_of_words = 0;
    entry_list my_entry_list = NULL;
    create_entry_list(&my_entry_list); 
    FILE* fp = fopen("queries.txt","r");
    if(fp == NULL){
        return NULL;
    }

    char c;
    char* query_word = NULL;
    word* my_word = NULL;
    int word_length = 0;
    while((c = fgetc (fp)) != EOF){

        if((c == ' ' || c == '\n') && (word_length > 0)){     //full word has been read. add letters to form the word
            //store word
            //move to next
            query_word[word_length] = '\0';
            if(number_of_words == 0){         //first word, initialize array
                my_word = (word*)malloc(sizeof(word));
                my_word->key_word = (char*)malloc(strlen(query_word)*sizeof(query_word));
                strcpy(my_word->key_word,query_word);
                entry word_entry = NULL;
                create_entry(my_word,&word_entry);
                add_entry(&my_entry_list,&word_entry);
                free(query_word);
                free_word(my_word);
                number_of_words++;
            }else{
                int exists = 0;
                //check that this word doesnt already exist in the array; deduplicate here
                entry* temp = get_first(&my_entry_list);
                entry temp_entry = *temp;
                while(temp_entry != NULL){
                    if(query_word != NULL && temp_entry->this_word->key_word != NULL){
                       if(strcmp(temp_entry->this_word->key_word,query_word)==0){
                           free(query_word);
                           exists = 1;
                           break;
                       }
                    }
                    temp_entry = temp_entry->next;
                }
                if(exists == 0){
                    //if it doesnt, append the array
                    my_word = (word*)malloc(sizeof(word));
                    my_word->key_word = (char*)malloc(strlen(query_word)*sizeof(query_word));
                    strcpy(my_word->key_word,query_word);
                    entry word_entry = NULL;
                    create_entry(my_word,&word_entry);
                    add_entry(&my_entry_list,&word_entry);
                    free(query_word);
                    free_word(my_word);
                    number_of_words++;
                }
            }
            word_length = 0;
            query_word = NULL;
            my_word = NULL;
        }else{
            word_length++;
            char* temp_word;
            if(query_word != NULL){
                temp_word = (char*)realloc(query_word,(word_length+1)*sizeof(char));
                if(temp_word == NULL){
                    free(query_word);
                    *number = 0;
                    return NULL;
                }
                query_word = temp_word;
            }else{
                query_word = (char*)realloc(query_word,(word_length+1)*sizeof(char));
            }
            query_word[word_length-1] = c;
        }
    }
    fclose(fp);
    *number = number_of_words;
    return my_entry_list;
}

entry_list read_queries(int* number,FILE* fp){       //max word length is 31
    int number_of_words = 0;
    entry_list my_entry_list = NULL;
    create_entry_list(&my_entry_list); 
    if(fp == NULL){
        return NULL;
    }

    char read_word[31];
    while(fscanf(fp," %31s",read_word) == 1){
        if(strcmp(read_word,"$") == 0){      //end of queries
            return my_entry_list;
        }
        //read word
        //check if it already exists in list
        entry* temp = get_first(&my_entry_list);
        entry temp_entry = *temp;
        int found = -1;
        while(temp_entry != NULL){
            if(strcmp(temp_entry->this_word->key_word,read_word)==0){
                found = 1;
                break;
            }
            temp_entry = temp_entry->next;
        }
        if(found == -1){
            //create word
            word* my_word = NULL;
            my_word = (word*)malloc(sizeof(word));
            my_word->key_word = (char*)malloc(strlen(read_word)*sizeof(read_word));
            strcpy(my_word->key_word,read_word);

            //create entry
            entry word_entry = NULL;
            create_entry(my_word,&word_entry);
            free_word(my_word);
            //add entry to list
            add_entry(&my_entry_list,&word_entry);
            number_of_words++;
        }
    }

    return my_entry_list;
}

int count_documents(FILE* fp){
    int number = 0;
    char read_word[31];
    while(fscanf(fp," %31s",read_word) == 1){
        if(strcmp(read_word,"$") == 0){      //end of queries
            number++;
        }
    }
    return number;
}

entry_list* read_documents(int* number,FILE* fp,int number_of_documents){       //max word length is 31
    
    entry_list* document_entry_list_array = malloc(sizeof(entry_list)*number_of_documents);
    int documents_read = 0;
    if(fp == NULL){
        return NULL;
    }
    char read_word[31];

    //ignore all queries, get to first document
    while(fscanf(fp," %31s",read_word) == 1){
        if(strcmp(read_word,"$") == 0){      //end of queries
            break;
        }
    }

    entry_list document_entry_list = NULL;
    create_entry_list(&document_entry_list); 

    while(fscanf(fp," %31s",read_word) != EOF){
        //read word
        //check if it already exists in list
        if(strcmp(read_word,"$") == 0){         //one full document has been read, move on to the next one
            //store it and move on if needed
            document_entry_list_array[documents_read] = document_entry_list;
            documents_read++;
            if(documents_read < number_of_documents){   //more documents to be read        //it is not the last one
                document_entry_list = NULL;
                create_entry_list(&document_entry_list);
                printf("i get here\n"); 
            }
        }else{                                  //keep adding words to current document
            entry* temp = get_first(&document_entry_list);
            entry temp_entry = *temp;
            int found = -1;
            while(temp_entry != NULL){              //search for this word
                if(strcmp(temp_entry->this_word->key_word,read_word) == 0){
                    found = 1;
                    break;
                }
                temp_entry = temp_entry->next;
            }
            if(found == -1){
                //create word
                word* my_word = NULL;
                my_word = (word*)malloc(sizeof(word));
                my_word->key_word = (char*)malloc(strlen(read_word)*sizeof(read_word));
                strcpy(my_word->key_word,read_word);

                //create entry
                entry word_entry = NULL;
                create_entry(my_word,&word_entry);
                free_word(my_word);
                //add entry to list
                add_entry(&document_entry_list,&word_entry);
            }
        }
    }
    
    //save the last document
    document_entry_list_array[documents_read] = document_entry_list;

    return document_entry_list_array;
    
}

void check_entry_list(const entry_list doc_list, bk_index* ix,int threshold){
    entry head = doc_list->first_node;
    while(head != NULL){
        entry_list el ;
        create_entry_list(&el);
        printf("%s\n",head->this_word->key_word);
        lookup_entry_index(head->this_word,ix,threshold,&el);
        if(el != NULL ){
            printf("Word %s matches with:\n",head->this_word->key_word);
            entry* result = get_first(&el);
            entry temp = *result;
            while(temp != NULL){
                printf("%s\n",temp->this_word->key_word);
                temp = temp->next;
            }
        }
        destroy_entry_list(&el);
        head = head->next;
    }
}

int bk_add_node_no_sort(bk_index* ix,word* entry_word,enum match_type type){
    bk_index temp_child  = (*ix)->child;
    int dist;

    if(type == EDIT_DIST){
        dist = editDist(entry_word->key_word,(*ix)->this_word->key_word,strlen(entry_word->key_word),strlen((*ix)->this_word->key_word));
    }else{
        dist = humming_distance(entry_word->key_word,(*ix)->this_word->key_word,strlen(entry_word->key_word),strlen((*ix)->this_word->key_word));
    }
    //if ix doesnt have children create a node and set it as his  child
    if(temp_child == NULL){

        bk_create_node(&(*ix)->child, entry_word, dist);
        return 1; 

    }else{

        //for every child of ix 
        while(temp_child != NULL){

            //if exists child with same dist go to this child , child
            if(temp_child->weight == dist){
                bk_add_node_no_sort(&temp_child ,entry_word,type);
                return 1;
            }
            //if we wave seen all ix children and there is no child with same distance
            if(temp_child->next == NULL){
                //add new node at the end of children
                bk_create_node(&(temp_child->next),entry_word,dist);
                return 1;
            }
            temp_child = temp_child -> next;
        }
    }
    return 1;
}

enum error_code build_entry_index_no_sort(const entry_list* el, enum match_type type, bk_index* ix){
    if((*el)->first_node == NULL){
        return NULL_POINTER;
    }
    entry temp_entry = (*el)->first_node;
    //create the root of the tree
    if((*ix) == NULL){
        bk_create_node(ix,temp_entry->this_word,0);
    }

    //for every word in the list add it to the tree
    temp_entry = temp_entry->next;
    while(temp_entry != NULL){
        bk_add_node_no_sort(ix,temp_entry->this_word,type);
        temp_entry = temp_entry->next;
    }

    return SUCCESS;
}
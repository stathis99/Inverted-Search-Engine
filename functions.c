#include "./include/structs.h"
#include <stdlib.h>
#include <string.h>


//entry functions
void free_word(word* w){
    free(w);
}

enum error_code create_entry(const word* w, entry* e){
    *e = malloc(sizeof(Entry));

    //duplicate the word
    //is this needed?
    // (*e)->this_word = malloc(strlen(w)+1);
    // strcpy((*e)->this_word,w);
    (*e)->this_word = (word*)w;
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
    free((*e)->this_word);
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

enum error_code add_entry_no_list(entry first, const entry new_entry){
    if(first == NULL){
        return NULL_POINTER;
    }
    new_entry->next = first->next;
    first->next = new_entry;
    return SUCCESS;
}


void print_list(const entry_list el){
    if(el == NULL){
        return;
    }
    entry head = el->first_node;
    while(head != NULL){
        printf("%s ",head->this_word);
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
int edit_dist(const char* str1, const char* str2, int len1, int len2){
    // Create a table to store results of subproblems
    int cost[len1 + 1][len2 + 1];
  
    // Fill d[][] in bottom up manner
    for (int i = 0; i <= len1; i++) {
        for (int j = 0; j <= len2; j++) {
            // If first string is empty, only option is to
            // insert all characters of second string
            if (i == 0)
                cost[i][j] = j; // Min. operations = j
  
            // If second string is empty, only option is to
            // remove all characters of second string
            else if (j == 0)
                cost[i][j] = i; // Min. operations = i
  
            // If last characters are same, ignore last char
            // and recur for remaining string
            else if (str1[i - 1] == str2[j - 1])
                cost[i][j] = cost[i - 1][j - 1];
  
            // If the last character is different, consider
            // all possibilities and find the minimum
            else
                cost[i][j]
                    = 1
                      + min3(cost[i][j - 1], // Insert
                            cost[i - 1][j], // Remove
                            cost[i - 1][j - 1]); // Replace
        }
    }
  
    return cost[len1][len2];
}

int humming_distance(const char* str1, const char* str2, int len){
    
    //compare characters one by one
    int distance = 0;
    
    for(int i=0; i < len; i++){
        if(str1[i] != str2[i]){
            distance++;
        }
    }
    return distance;
}


//bk functions
 void print_bk_tree(bk_index ix, int pos){  

    if(ix == NULL){
        return;
    }  

    bk_index temp_child  = ix->child;
    
    printf("\n\n   %s:%d  %d \n",ix->this_word,ix->weight,pos);

    Payload* temp_payload;
    temp_payload = ix->payload;
    do{
        printf("q:%d t:%d\n\n",temp_payload->queryId,temp_payload->threshold);
        temp_payload = temp_payload->next;
    }while(temp_payload != NULL);

    while(temp_child != NULL){
        print_bk_tree(temp_child,pos-1);
        temp_child = temp_child->next;   
    }
    return;
 }

//creates a bk tree node
bk_index bk_create_node(bk_index* ix,word* entry_word,int weight,int queryId, int dist){

        (*ix) = malloc(sizeof(Index));
        (*ix)->weight = weight;
        (*ix)->this_word = entry_word;
        (*ix)->next = NULL;
        (*ix)-> child = NULL;

        //this is for payload might change
        (*ix)->payload = malloc(sizeof(Payload));
        (*ix)->payload->next = NULL;
        (*ix)->payload->queryId = queryId;
        (*ix)->payload->threshold = dist;

        return *ix;
}

enum error_code lookup_entry_index(const word* w, bk_index* ix, int threshold, entry_list* result){
    bk_index temp_child  = (*ix)->child;
    int dist = edit_dist(w,(*ix)->this_word,strlen(w),strlen((*ix)->this_word));
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
    }
    
    return SUCCESS;
}

enum error_code destroy_entry_index(bk_index* ix){
    if(*ix == NULL){
        return NULL_POINTER;
    }

    bk_index temp_child  = (*ix)->child;
    //temporary because of unnecessary word* struct
    free((*ix)->this_word);
    Payload* curr;
    while((*ix)->payload != NULL){
	    curr = (*ix)->payload;
	    (*ix)->payload = (*ix)->payload->next;
	free(curr);
    }
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

void check_entry_list(const entry_list doc_list, bk_index* ix,int threshold){
    entry head = doc_list->first_node;
    while(head != NULL){
        entry_list el ;
        create_entry_list(&el);
        lookup_entry_index(head->this_word,ix,threshold,&el);
        if(el != NULL ){
            printf("Word %s matches with:\n",head->this_word);
            entry* result = get_first(&el);
            entry temp = *result;
            while(temp != NULL){
                printf("%s\n",temp->this_word);
                temp = temp->next;
            }
        }
        destroy_entry_list(&el);
        head = head->next;
    }
}

int bk_add_node(bk_index* ix,word* entry_word,enum match_type type, bk_index* node, int queryId, int threshold){
    bk_index temp_child  = (*ix)->child;
    int dist;

    if(type == EDIT_DIST){
        dist = edit_dist(entry_word,(*ix)->this_word,strlen(entry_word),strlen((*ix)->this_word));
    }else{
        dist = humming_distance(entry_word,(*ix)->this_word,strlen((*ix)->this_word));
    }
    //if ix doesnt have children create a node and set it as his  child
    if(temp_child == NULL){

        *node = bk_create_node(&(*ix)->child, entry_word, dist,queryId,threshold);
        return 1; 

    }else{

        //for every child of ix 
        while(temp_child != NULL){

            //if exists child with same dist go to this child , child
            if(temp_child->weight == dist){
                bk_add_node(&temp_child ,entry_word,type, node,queryId,dist);
                return 1;
            }
            //if we wave seen all ix children and there is no child with same distance
            if(temp_child->next == NULL){
                //add new node at the end of children
                *node = bk_create_node(&(temp_child->next),entry_word,dist,queryId,threshold);
                return 1;
            }
            temp_child = temp_child -> next;
        }
    }
    return 1;
}


//adds new payload to payload list
void add_payload(struct Payload* payload,int queryId, int dist){
    Payload* new_payload = malloc(sizeof(Payload));
    new_payload->queryId = queryId;
    new_payload->threshold = dist;
    new_payload->next = payload->next;
    payload->next = new_payload;
}



void deduplicate_edit_distance(const char* temp, unsigned int queryId, int dist, int type, Hash_table** hash_table, bk_index* ix){
    char* read_word;
    char* temp_temp = (char*)temp; 
    read_word = strtok(temp_temp, " ");
    while(read_word != NULL){
        int len = strlen(read_word);
        printf("%s %d %d %d \n",read_word ,queryId, dist, type );
        
        int word_hash_value = hash(read_word)%10;
        printf("Word %s hashes to %d\n",read_word,word_hash_value);
        if(hash_table[len-1] == NULL){

            //initialize hashtable 
            hash_table[len-1] = malloc(sizeof(Hash_table));
            hash_table[len-1]->hash_buckets = malloc(sizeof(Hash_Bucket*)*10);
            
            //initialize hash buckets
            for(int i=0 ; i<= 9 ;i++){
                hash_table[len-1]->hash_buckets[i] = NULL;
            }
            //hash word to find bucket
            hash_table[len-1]->hash_buckets[word_hash_value] = malloc(sizeof(Hash_Bucket));

            if((*ix) == NULL){                  //bk root hasnt been initialized
                word* my_word= malloc(strlen(read_word)+1);
                strcpy(my_word,read_word);

                bk_create_node(ix,my_word,0,queryId,dist);
                hash_table[len-1]->hash_buckets[word_hash_value]->node = *ix;
                hash_table[len-1]->hash_buckets[word_hash_value]->next = NULL;

            }else{
                word* my_word= malloc(strlen(read_word)+1);
                strcpy(my_word,read_word);

                bk_index node = NULL;
                bk_add_node(ix, my_word, 1, &node, queryId,dist);
                hash_table[len-1]->hash_buckets[word_hash_value]->node = node;
                hash_table[len-1]->hash_buckets[word_hash_value]->next = NULL;
            }
            

        }else{              //this hash table has been initialized
        
            //search if it exists in any of the buckets, starting from the first
            Hash_Bucket* current = hash_table[len-1]->hash_buckets[word_hash_value];
            if(hash_table[len-1]->hash_buckets[word_hash_value] == NULL){
                //first word for this bucket, create the bucket
                hash_table[len-1]->hash_buckets[word_hash_value] = malloc(sizeof(Hash_Bucket));

                word* my_word= malloc(strlen(read_word)+1);
                strcpy(my_word,read_word);

                bk_index node = NULL;
                bk_add_node(ix, my_word, 1, &node,queryId,dist);
                hash_table[len-1]->hash_buckets[word_hash_value]->node = node;
                hash_table[len-1]->hash_buckets[word_hash_value]->next = NULL;
            }else{
                int found = -1;
                while (current != NULL){
                    if(strcmp(current->node->this_word,read_word) == 0){       //does the word exist
                        found = 1;
                        //printf("I found word %s again\n",read_word);
                        add_payload(current->node->payload,queryId,dist);
                        break;
                    }
                    current = current->next;
                }
                if(found == -1){                // it was not found, we shall add it and add it to the list of buckets
                    word* my_word= malloc(strlen(read_word)+1);
                    strcpy(my_word,read_word);

                    bk_index node = NULL;
                    bk_add_node(ix, my_word, 1, &node,queryId,dist);
                    //create this hash_bucket to store data
                    Hash_Bucket* new_bucket = malloc(sizeof(Hash_Bucket));
                    new_bucket->node = node;
                    new_bucket->next = NULL;

                    //append it to the head
                    Hash_Bucket* previous_first = hash_table[len-1]->hash_buckets[word_hash_value];
                    //first bucket is now the new one
                    hash_table[len-1]->hash_buckets[word_hash_value] = new_bucket;
                    new_bucket->next = previous_first;


                }
            }
        }
   


        //processing of current word ended, move on to the next one
        read_word = strtok(NULL, " ");
    }

}

unsigned long hash(unsigned char *str){
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void print_hash_tables(Hash_table** hash_table){
    for(int i=0 ; i <= 28; i++){
        if(hash_table[i] != NULL){
            for(int j=0; j <= 9; j++){
                Hash_Bucket* current = hash_table[i]->hash_buckets[j];
                while(current != NULL){
                    printf("Printing length %d hash %d:\n",i+1,j);
                    //access the bk node that this bucket points to
                    printf("%s     ->",current->node->this_word);
                    current = current->next;
                }printf("\n");
            }
            printf("Length %d finished\n\n",i+1);
        }
    }
}

void delete_hash_tables_edit(Hash_table** hash_tables){
    for(int i=0; i <= 28; i++){
        //if this hash table has been allocated, free it; else move on
        if(hash_tables[i] != NULL){
            for(int j=0; j <= 9; j++){
                //Hash_Bucket* current = hash_tables[i]->hash_buckets[j];
                    Hash_Bucket* temp ; 
                    while(hash_tables[i]->hash_buckets[j] != NULL){        //if there exists at least one bucket, free it and every next it has
                        temp = hash_tables[i]->hash_buckets[j];
                        hash_tables[i]->hash_buckets[j] = hash_tables[i]->hash_buckets[j]->next;
                        free(temp); 
                    }
            }
            free(hash_tables[i]->hash_buckets);
            free(hash_tables[i]);
        }
    }
    free(hash_tables);
}

unsigned int djb2(const void *_str) {
	const char *str = _str;
	unsigned int hash = 5381;
	char c;
	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

unsigned int jenkins(const void *_str) {
	const char *key = _str;
	unsigned int hash = 0;
	while (*key) {
		hash += *key;
		hash += (hash << 10);
		hash ^= (hash >> 6);
		key++;
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}



void deduplicate_exact_matching(const char* temp, unsigned int queryId, int dist, int type, Hash_table_exact** hash_table_exact, int bloom_filter[]){
    
    char* read_word;
    char* temp_temp = (char*)temp; 
    read_word = strtok(temp_temp, " ");
    while(read_word != NULL){
        int len = strlen(read_word);
        printf("%s %d %d %d \n",read_word ,queryId, dist, type );
        int word_hash_value1 = jenkins(read_word)%100;
        int word_hash_value2 = djb2(read_word)%100;

        bloom_filter[word_hash_value1] = 1;
        bloom_filter[word_hash_value2] = 1;

        int word_hash_value = hash(read_word)%10;
        printf("Word %s hashes to j:%d dj:%d\n",read_word,word_hash_value1,word_hash_value2);
        
        if(hash_table_exact[len-1] == NULL){

            //initialize hashtable 
            hash_table_exact[len-1] = malloc(sizeof(Hash_table_exact));
            hash_table_exact[len-1]->hash_buckets = malloc(sizeof(entry)*10);

            //initialize hash buckets
            for(int i=0 ; i<= 9 ;i++){
                hash_table_exact[len-1]->hash_buckets[i] = NULL;
            }
            word* my_word= malloc(strlen(read_word)+1);
            strcpy(my_word,read_word);

            create_entry(my_word,&hash_table_exact[len-1]->hash_buckets[word_hash_value]);
            //free_word(my_word);

        }else{              //this hash table has been initialized
        
            entry current = hash_table_exact[len-1]->hash_buckets[word_hash_value];
            if(hash_table_exact[len-1]->hash_buckets[word_hash_value] == NULL){

                word* my_word= malloc(strlen(read_word)+1);
                strcpy(my_word,read_word);

                create_entry(my_word,&hash_table_exact[len-1]->hash_buckets[word_hash_value]);
                //free_word(my_word);
            }else{
                int found = -1;
                while (current != NULL){
                    if(strcmp(current->this_word,read_word) == 0){       //does the word exist
                        found = 1;
                        printf("I found word %s again\n",read_word);
                        break;
                    }
                    current = current->next;
                }
                if(found == -1){                // it was not found, we shall add it and add it to the list of buckets
                    word* my_word= malloc(strlen(read_word)+1);
                    strcpy(my_word,read_word);

                    entry temp_entry = NULL;

                    create_entry(my_word,&temp_entry);
                    add_entry_no_list(hash_table_exact[len-1]->hash_buckets[word_hash_value],temp_entry);
                    //free_word(my_word);
                }
            }
        }
        //processing of current word ended, move on to the next one
        read_word = strtok(NULL, " ");
    }

}

void deduplicate_humming(const char* temp, unsigned int queryId, int dist, int type, Hash_table** hash_table, bk_index* humming_root_table){


    char* read_word;
    char* temp_temp = (char*)temp; 
    read_word = strtok(temp_temp, " ");
    while(read_word != NULL){
        int len = strlen(read_word);
        printf("%s %d %d %d \n",read_word ,queryId, dist, type );
        
        int word_hash_value = hash(read_word)%10;
        printf("Word %s hashes to %d\n",read_word,word_hash_value);
        if(hash_table[len-1] == NULL){

            //initialize hashtable 
            hash_table[len-1] = malloc(sizeof(Hash_table));
            hash_table[len-1]->hash_buckets = malloc(sizeof(Hash_Bucket*)*10);
            
            //initialize hash buckets
            for(int i=0 ; i<= 9 ;i++){
                hash_table[len-1]->hash_buckets[i] = NULL;
            }
            //hash word to find bucket
            hash_table[len-1]->hash_buckets[word_hash_value] = malloc(sizeof(Hash_Bucket));

            if(humming_root_table[len-1] == NULL){                  //bk root hasnt been initialized
                word* my_word= malloc(strlen(read_word)+1);
                strcpy(my_word,read_word);

                bk_create_node(&humming_root_table[len-1],my_word,0,queryId,dist);
                hash_table[len-1]->hash_buckets[word_hash_value]->node = humming_root_table[len-1];
                hash_table[len-1]->hash_buckets[word_hash_value]->next = NULL;
            }else{
                //unnecessary, just fix struct word
                word* my_word= malloc(strlen(read_word)+1);
                strcpy(my_word,read_word);

                bk_index node = NULL;
                bk_add_node(&humming_root_table[len-1], my_word, 1, &node,queryId,dist);
                hash_table[len-1]->hash_buckets[word_hash_value]->node = node;
                hash_table[len-1]->hash_buckets[word_hash_value]->next = NULL;
            }
            

        }
        else{              //this hash table has been initialized
        
            //search if it exists in any of the buckets, starting from the first
            Hash_Bucket* current = hash_table[len-1]->hash_buckets[word_hash_value];
            if(hash_table[len-1]->hash_buckets[word_hash_value] == NULL){
                //first word for this bucket, create the bucket
                hash_table[len-1]->hash_buckets[word_hash_value] = malloc(sizeof(Hash_Bucket));

                word* my_word= malloc(strlen(read_word)+1);
                strcpy(my_word,read_word);

                bk_index node = NULL;
                bk_add_node(&humming_root_table[len-1], my_word, 1, &node,queryId,dist);
                hash_table[len-1]->hash_buckets[word_hash_value]->node = node;
                hash_table[len-1]->hash_buckets[word_hash_value]->next = NULL;
            }else{
                int found = -1;
                while (current != NULL){
                    if(strcmp(current->node->this_word,read_word) == 0){       //does the word exist
                        found = 1;
                        add_payload(current->node->payload,queryId,dist);
                        printf("I found word %s again\n",read_word);
                        break;
                    }
                    current = current->next;
                }
                if(found == -1){                // it was not found, we shall add it and add it to the list of buckets
                    word* my_word= malloc(strlen(read_word)+1);
                    strcpy(my_word,read_word);

                    bk_index node = NULL;
                    bk_add_node(&humming_root_table[len-1], my_word, 1, &node,queryId,dist);
                    //create this hash_bucket to store data
                    Hash_Bucket* new_bucket = malloc(sizeof(Hash_Bucket));
                    new_bucket->node = node;
                    new_bucket->next = NULL;

                    //append it to the head
                    Hash_Bucket* previous_first = hash_table[len-1]->hash_buckets[word_hash_value];
                    //first bucket it now the new one
                    hash_table[len-1]->hash_buckets[word_hash_value] = new_bucket;
                    new_bucket->next = previous_first;


                }
            }
        }
   


        //processing of current word ended, move on to the next one
        read_word = strtok(NULL, " ");
    }

}

void print_hash_table_exact(Hash_table_exact** hash_table_exact){
    for(int i=0; i<=28; i++){
        if(hash_table_exact[i] != NULL){
            printf("\nPrinting len %d\n",i);
            for(int j=0; j< 10; j++){
                if( hash_table_exact[i]->hash_buckets[j] != NULL){
                    printf("Printing bucket %d\n",j);
                    entry temp = hash_table_exact[i]->hash_buckets[j]; 
                    while(temp !=NULL ){
                        printf("%s     ->",temp->this_word);
                        temp =temp->next;
                    }
                }
            }
        }
    }
}

void delete_hash_tables_humming(Hash_table** hash_tables, bk_index* humming_root_table){
    
    for(int i=0; i <= 28; i++){
        //if this hash table has been allocated, free it; else move on
        if(hash_tables[i] != NULL){
            for(int j=0; j <= 9; j++){
                //free the hash_table itself
                Hash_Bucket* temp ; 
                while(hash_tables[i]->hash_buckets[j] != NULL){        //if there exists at least one bucket, free it and every next it has
                    temp = hash_tables[i]->hash_buckets[j];
                    hash_tables[i]->hash_buckets[j] = hash_tables[i]->hash_buckets[j]->next;
                    free(temp); 
                }
            }
            free(hash_tables[i]->hash_buckets);
            free(hash_tables[i]);

            //if hash table has been initialized then we have at least a root for a bk of this length
            //free the bk trees starting from their roots
            destroy_entry_index(&humming_root_table[i]);
        }
    }

    //finally, free both arrays
    free(hash_tables);
    free(humming_root_table);
}

void delete_hash_tables_exact(Hash_table_exact** hash_tables_exact){
    for(int i=0; i <= 28; i++){
        //if this hash table has been allocated, free it; else move on
        if(hash_tables_exact[i] != NULL){
            for(int j=0; j <= 9; j++){
                //free the hash_table itself
                entry temp ; 
                while(hash_tables_exact[i]->hash_buckets[j] != NULL){        //if there exists at least one bucket, free it and every next it has
                    temp = hash_tables_exact[i]->hash_buckets[j];
                    hash_tables_exact[i]->hash_buckets[j] = hash_tables_exact[i]->hash_buckets[j]->next;
                    destroy_entry(&temp); 
                }
            }
            free(hash_tables_exact[i]->hash_buckets);
            free(hash_tables_exact[i]);
        }
    }
    free(hash_tables_exact);
}
#include "../include/core.h"

#include <stdlib.h>
#include <time.h> 
#include <string.h>

//global structs we are using

//bloom filter used in exact match
int bloom_filter_exact[BLOOM_FILTER_SIZE];

Query_Hash_Table* Q_Hash_Table;

bk_index ix;
Hash_table* hash_tables_edit;

bk_index* hamming_root_table;
Hash_table** hash_tables_hamming;

Hash_table_exact** hash_table_exact;

result_node* r_node= NULL;
result_node_bk* r_node_bk_hamming = NULL;
result_node_bk* r_node_bk_edit = NULL;

Words_Hash_Table* words_hash_table = NULL;
int last_doc_id;

Batch_results* batch_results;
Batch_results_list* batch_results_list;

//----------------------------------------distance functions

//calculates levestein distance
static int distance (const char * word1,int len1, const char * word2,int len2){

    int matrix[len1 + 1][len2 + 1];
    int i;
    for (i = 0; i <= len1; i++) {
        matrix[i][0] = i;
    }
    for (i = 0; i <= len2; i++) {
        matrix[0][i] = i;
    }
    for (i = 1; i <= len1; i++) {
        int j;
        char c1;

        c1 = word1[i-1];
        for (j = 1; j <= len2; j++) {
            char c2;

            c2 = word2[j-1];
            if (c1 == c2) {
                matrix[i][j] = matrix[i-1][j-1];
            }
            else {
                int delete;
                int insert;
                int substitute;
                int minimum;

                delete = matrix[i-1][j] + 1;
                insert = matrix[i][j-1] + 1;
                substitute = matrix[i-1][j-1] + 1;
                minimum = delete;
                if (insert < minimum) {
                    minimum = insert;
                }
                if (substitute < minimum) {
                    minimum = substitute;
                }
                matrix[i][j] = minimum;
            }
        }
    }

    return matrix[len1][len2];
}

int hamming_distance(const char* str1, const char* str2, int len){
    
    //compare characters one by one
    int distance = 0;
    
    for(int i=0; i < len; i++){
        if(str1[i] != str2[i]){
            distance++;
        }
    }
    return distance;
}

//----------------------------------------hash functions

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

unsigned long hash(unsigned char *str){
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

    return hash;
}


//---------------------------------------Entry functions. Entry stores a word and has a payload with all the queries that contain this word.


enum error_code create_entry(const word* w, entry* e,unsigned int queryId,int dist){
    *e = malloc(sizeof(Entry));

    //(*e)->this_word = (word*)w;
    strcpy((*e)->this_word, w);
    (*e)->payload = malloc(sizeof(Payload));
    (*e)->payload->queryId = queryId;
    (*e)->payload->threshold = dist;
    (*e)->payload->next = NULL;

    (*e)->next = NULL;
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

enum error_code destroy_entry(entry* e){
    if(*e == NULL){
        return NULL_POINTER;
    }
    
    Payload* curr;
    while((*e)->payload != NULL){
	    curr = (*e)->payload;
	    (*e)->payload = (*e)->payload->next;
	    free(curr);
    }

    free(*e);
    return SUCCESS;
}

//adds new payload to payload list
void add_payload(struct Payload* payload,int queryId, int dist){
    Payload* new_payload = malloc(sizeof(Payload));
    new_payload->queryId = queryId;
    new_payload->threshold = dist;
    new_payload->next = payload->next;
    payload->next = new_payload;
}


//-----------------------------------------BK tree functions. BK tree stores querie words and is used for searching matches with a given threshold

//creates a bk tree node
bk_index bk_create_node(bk_index* ix,const word* entry_word,int weight,int queryId, int dist){

        (*ix) = malloc(sizeof(Index));
        (*ix)->weight = weight;
        (*ix)->len = strlen(entry_word);
        strcpy((*ix)->this_word, entry_word);
        (*ix)->next = NULL;
        (*ix)-> child = NULL;

        //this is for payload might change
        (*ix)->payload = malloc(sizeof(Payload));
        (*ix)->payload->queryId = queryId;
        (*ix)->payload->threshold = dist;
        (*ix)->payload->next = NULL;

        return *ix;
}

//looks for queries in paload list of a word and updates the result list. this function is called if a doc word matches with a word in a bk tree.
enum error_code look_for_threshold_hamming(struct Payload* payload,int threshold,const word* q_w,const word* w,bk_index temp){
    
    while(payload != NULL){
        if(payload->threshold == threshold){
                if (strcmp(temp->this_word,q_w) == 0){
                        
                        if(r_node_bk_hamming == NULL){
                            r_node_bk_hamming = malloc(sizeof(result_node_bk));
                            r_node_bk_hamming->next = NULL;
                            r_node_bk_hamming->this_entry = temp;                         
                        }else{
                            result_node_bk* current = r_node_bk_hamming;
                            int found = -1;
                            while(current != NULL){
                                if(strcmp(current->this_entry->this_word,q_w) == 0){
                                   found = 1;
                                    break;
                                }
                                current = current->next;
                            }
                            if(found == -1){
                                result_node_bk* new_node = malloc(sizeof(result_node_bk));
                                new_node->next = NULL;
                                new_node->this_entry = temp; 

                                result_node_bk* second = r_node_bk_hamming->next;
                                r_node_bk_hamming->next = new_node;
                                new_node->next = second;                
                            }
                        }                         
                    }

        }
        payload = payload->next;
    }
}

enum error_code look_for_threshold_edit(struct Payload* payload,int threshold,const word* q_w,const word* w,bk_index temp){
    
    while(payload != NULL){
        if(payload->threshold == threshold){
                     
                if (strcmp(temp->this_word,q_w) == 0){

                            if(r_node_bk_edit == NULL){
                                r_node_bk_edit = malloc(sizeof(result_node_bk));
                                r_node_bk_edit->next = NULL;
                                r_node_bk_edit->this_entry = temp;   
                                r_node_bk_edit->my_threshold = threshold;                      
                            }else{
                                result_node_bk* current = r_node_bk_edit;
                                int found = -1;
                                while(current != NULL){
                                    if(strcmp(current->this_entry->this_word,q_w) == 0){
                                        if(current->my_threshold > threshold){
                                             current->my_threshold = threshold;   
                                        }
                                        found = 1;
                                        break;
                                    }
                                    current = current->next;
                                }
                                if(found == -1){
                                    result_node_bk* new_node = malloc(sizeof(result_node_bk));
                                    new_node->next = NULL;
                                    new_node->this_entry = temp;
                                    new_node->my_threshold = threshold; 

                                    result_node_bk* second = r_node_bk_edit->next;
                                    r_node_bk_edit->next = new_node;
                                    new_node->next = second;                
                                }
                            }                         
                        }
        }
        payload = payload->next;
    }
}

//looks for words that match with a given doc word in a bk tree 
enum error_code lookup_entry_index_edit(const word* w,int docWordLen, bk_index* ix, int threshold){
    bk_index temp_child  = (*ix)->child;

    int dist = distance(w,docWordLen,(*ix)->this_word,(*ix)->len);

    //if ix keyword is close to doc word then look for queries id in payload list that match the given threshold
    if( dist <= threshold ){

        look_for_threshold_edit((*ix)->payload,threshold,(*ix)->this_word,w,*ix);
    }

    //traverse the tree retrospectively 
    while(temp_child != NULL){
        //if distance child from the parent is  [𝑑 − 𝑛, 𝑑 + 𝑛] then call retrospectively 
        if(temp_child->weight >= dist - threshold && temp_child->weight <= dist + threshold ){
            lookup_entry_index_edit(w,docWordLen,&temp_child,threshold);
        }
        temp_child = temp_child->next;   
    }
    
    return SUCCESS;
}

enum error_code lookup_entry_index_hamming(const word* w,int docWordLen, bk_index* ix, int threshold){
    bk_index temp_child  = (*ix)->child;

    int dist = hamming_distance(w,(*ix)->this_word,docWordLen);

    //if ix keyword is close to doc word then look for queries id in payload list that match the given threshold
    if( dist <= threshold ){

        look_for_threshold_hamming((*ix)->payload,threshold,(*ix)->this_word,w,*ix);
    }

    //traverse the tree retrospectively 
    while(temp_child != NULL){
        //if distance child from the parent is  [𝑑 − 𝑛, 𝑑 + 𝑛] then call retrospectively 
        if(temp_child->weight >= dist - threshold && temp_child->weight <= dist + threshold ){
            lookup_entry_index_hamming(w,docWordLen,&temp_child,threshold);
        }
        temp_child = temp_child->next;   
    }
    
    return SUCCESS;
}

//looks for ward in exact match index
void lookup_exact(const word* w,Hash_table_exact** hash_table_exact){
    for(int i=0; i<=28; i++){
        if(hash_table_exact[i] != NULL){
            for(int j=0; j< EXACT_HASH_BUCKET; j++){
                if( hash_table_exact[i]->hash_buckets[j] != NULL){
                    entry temp = hash_table_exact[i]->hash_buckets[j]; 
                    while(temp != NULL){
                        if (strcmp(temp->this_word,w) == 0){
                            //printf("%s     ->\n\n",temp->this_word);
                            if(r_node == NULL){
                                r_node = malloc(sizeof(result_node));
                                r_node->next = NULL;
                                r_node->this_entry = temp;
                            }else{
                                result_node* current = r_node;
                                int found = -1;
                                while(current != NULL){
                                    if(strcmp(current->this_entry->this_word,w) == 0){
                                        found = 1;
                                        break;
                                    }
                                    current = current->next;
                                }
                                if(found == -1){
                                    result_node* new_node = malloc(sizeof(result_node));
                                    new_node->next = NULL;
                                    new_node->this_entry = temp; 

                                    result_node* second = r_node->next;
                                    r_node->next = new_node;
                                    new_node->next = second;
                                }
                            }                         
                        }
                        temp =temp->next;
                    }
                }
            }
        }
    }
}

//destroys a bk tree
enum error_code destroy_entry_index(bk_index* ix){
    if(*ix == NULL){
        return NULL_POINTER;
    }
    bk_index temp_child  = (*ix)->child;

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

//adds a node to a given bk tree
int bk_add_node_edit(bk_index* ix, const word* entry_word,int qWordLen, bk_index* node, int queryId, int threshold){
    bk_index temp_child  = (*ix)->child;

    int dist = distance(entry_word,qWordLen,(*ix)->this_word,(*ix)->len);

    //if ix doesnt have children create a node and set it as his  child
    if(temp_child == NULL){
        *node = bk_create_node(&(*ix)->child, entry_word, dist,queryId,threshold);
        return 1; 
    }else{
        //for every child of ix 
        while(temp_child != NULL){

            //if exists child with same dist go to this child , child
            if(temp_child->weight == dist){
                bk_add_node_edit(&temp_child ,entry_word,qWordLen, node,queryId,threshold);
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

int bk_add_node_hamming(bk_index* ix, const word* entry_word,int qWordLen, bk_index* node, int queryId, int threshold){
    bk_index temp_child  = (*ix)->child;


    int dist = hamming_distance(entry_word,(*ix)->this_word,qWordLen);

    //if ix doesnt have children create a node and set it as his  child
    if(temp_child == NULL){
        *node = bk_create_node(&(*ix)->child, entry_word, dist,queryId,threshold);
        return 1; 
    }else{
        //for every child of ix 
        while(temp_child != NULL){

            //if exists child with same dist go to this child , child
            if(temp_child->weight == dist){
                bk_add_node_hamming(&temp_child ,entry_word,qWordLen, node,queryId,threshold);
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


//prints a bk tree
 void print_bk_tree(bk_index ix, int pos){  

    if(ix == NULL){
        return;
    }  

    bk_index temp_child  = ix->child;
    
    printf("\n\n   %s:%d  %d     ",ix->this_word,ix->weight,pos);

    Payload* temp_payload;
    temp_payload = ix->payload;
    do{
        printf("q:%d t:%d ",temp_payload->queryId,temp_payload->threshold);
        temp_payload = temp_payload->next;
    }while(temp_payload != NULL);

    while(temp_child != NULL){
        print_bk_tree(temp_child,pos-1);
        temp_child = temp_child->next;   
    }
    return;
 }


//------------------------------------------deduplicate functions reciece all querie wards deduplicate them and add them to their indexes


void deduplicate_edit_distance(const char* temp, unsigned int queryId, int dist, int type, bk_index* ix){
    char* read_word;
    char* temp_temp = (char*)temp; 
    read_word = strtok(temp_temp, " ");
    while(read_word != NULL){
        int len = strlen(read_word);

        int word_hash_value = djb2(read_word)%EDIT_HASH_BUCKETS;
        
        if(hash_tables_edit->hash_buckets[word_hash_value] == NULL){
            //first word here, add it
            hash_tables_edit->hash_buckets[word_hash_value] = malloc(sizeof(Hash_Bucket));
            hash_tables_edit->hash_buckets[word_hash_value]->next = NULL;
            if((*ix) == NULL){                  //bk root hasnt been initialized

                bk_create_node(ix,read_word,0,queryId,dist);
                hash_tables_edit->hash_buckets[word_hash_value]->node = *ix;

            }else{

                bk_index node = NULL;
                bk_add_node_edit(ix, read_word,len, &node, queryId,dist);
                hash_tables_edit->hash_buckets[word_hash_value]->node = node;
            }
        }else{
            Hash_Bucket* current = hash_tables_edit->hash_buckets[word_hash_value];
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

                bk_index node = NULL;
                bk_add_node_edit(ix, read_word,len, &node,queryId,dist);
                //create this hash_bucket to store data
                Hash_Bucket* new_bucket = malloc(sizeof(Hash_Bucket));
                new_bucket->node = node;
                new_bucket->next = NULL;

                //append it to the head
                Hash_Bucket* previous_first = hash_tables_edit->hash_buckets[word_hash_value];
                //first bucket is now the new one
                hash_tables_edit->hash_buckets[word_hash_value] = new_bucket;
                new_bucket->next = previous_first;
            }
        }
        read_word = strtok(NULL, " ");
    }
}

void deduplicate_exact_matching(const char* temp, unsigned int queryId, int dist, int type){
    
    char* read_word;
    char* temp_temp = (char*)temp; 
    read_word = strtok(temp_temp, " ");
    while(read_word != NULL){
        int len = strlen(read_word);
        //printf("%s %d %d %d \n",read_word ,queryId, dist, type );

        bloom_filter_exact[djb2(read_word)%BLOOM_FILTER_SIZE] = 1;
        bloom_filter_exact[jenkins(read_word)%BLOOM_FILTER_SIZE] = 1;


        int word_hash_value = hash(read_word)%EXACT_HASH_BUCKET;
        //printf("Word %s hashes to j:%d dj:%d\n",read_word,word_hash_value1,word_hash_value2);
        
        if(hash_table_exact[len-1] == NULL){

            //initialize hashtable 
            hash_table_exact[len-1] = malloc(sizeof(Hash_table_exact));
            hash_table_exact[len-1]->hash_buckets = malloc(sizeof(entry)*EXACT_HASH_BUCKET);

            //initialize hash buckets
            for(int i=0 ; i<= EXACT_HASH_BUCKET-1 ;i++){
                hash_table_exact[len-1]->hash_buckets[i] = NULL;
            }

            create_entry(read_word,&hash_table_exact[len-1]->hash_buckets[word_hash_value],queryId,dist);

        }else{              //this hash table has been initialized
        
            entry current = hash_table_exact[len-1]->hash_buckets[word_hash_value];
            if(hash_table_exact[len-1]->hash_buckets[word_hash_value] == NULL){

                create_entry(read_word,&hash_table_exact[len-1]->hash_buckets[word_hash_value],queryId,dist);
            }else{
                int found = -1;
                while (current != NULL){
                    if(strcmp(current->this_word,read_word) == 0){       //does the word exist
                        found = 1;
                        add_payload(current->payload,queryId,dist);
                        //printf("I found word %s again\n",read_word);
                        break;
                    }
                    current = current->next;
                }
                if(found == -1){                // it was not found, we shall add it and add it to the list of buckets

                    entry temp_entry = NULL;

                    create_entry(read_word,&temp_entry,queryId,dist);
                    add_entry_no_list(hash_table_exact[len-1]->hash_buckets[word_hash_value],temp_entry);
                }
            }
        }
        //processing of current word ended, move on to the next one
        read_word = strtok(NULL, " ");
    }

}

void deduplicate_hamming(const char* temp, unsigned int queryId, int dist, int type){


    char* read_word;
    char* temp_temp = (char*)temp; 
    read_word = strtok(temp_temp, " ");
    while(read_word != NULL){
        int len = strlen(read_word);
        //printf("%s %d %d %d \n",read_word ,queryId, dist, type );

        int word_hash_value = hash(read_word)%HAMMING_HASH_BUCKET;
        //printf("Word %s hashes to %d\n",read_word,word_hash_value);
        if(hash_tables_hamming[len-1] == NULL){

            //initialize hashtable 
            hash_tables_hamming[len-1] = malloc(sizeof(Hash_table));
            hash_tables_hamming[len-1]->hash_buckets = malloc(sizeof(Hash_Bucket*)*HAMMING_HASH_BUCKET);
            
            //initialize hash buckets
            for(int i=0 ; i<= HAMMING_HASH_BUCKET-1 ;i++){
                hash_tables_hamming[len-1]->hash_buckets[i] = NULL;
            }
            //hash word to find bucket
            hash_tables_hamming[len-1]->hash_buckets[word_hash_value] = malloc(sizeof(Hash_Bucket));

            if(hamming_root_table[len-1] == NULL){                  //bk root hasnt been initialized

                bk_create_node(&hamming_root_table[len-1],read_word,0,queryId,dist);
                hash_tables_hamming[len-1]->hash_buckets[word_hash_value]->node = hamming_root_table[len-1];
                hash_tables_hamming[len-1]->hash_buckets[word_hash_value]->next = NULL;
            }else{
                //unnecessary, just fix struct word

                bk_index node = NULL;
                bk_add_node_hamming(&hamming_root_table[len-1],read_word,len, &node,queryId,dist);
                hash_tables_hamming[len-1]->hash_buckets[word_hash_value]->node = node;
                hash_tables_hamming[len-1]->hash_buckets[word_hash_value]->next = NULL;
            }
            

        }else{              //this hash table has been initialized
        
            //search if it exists in any of the buckets, starting from the first
            Hash_Bucket* current = hash_tables_hamming[len-1]->hash_buckets[word_hash_value];
            if(hash_tables_hamming[len-1]->hash_buckets[word_hash_value] == NULL){
                //first word for this bucket, create the bucket
                hash_tables_hamming[len-1]->hash_buckets[word_hash_value] = malloc(sizeof(Hash_Bucket));

                bk_index node = NULL;
                bk_add_node_hamming(&hamming_root_table[len-1], read_word,len, &node,queryId,dist);
                hash_tables_hamming[len-1]->hash_buckets[word_hash_value]->node = node;
                hash_tables_hamming[len-1]->hash_buckets[word_hash_value]->next = NULL;
            }else{
                int found = -1;
                while (current != NULL){
                    if(strcmp(current->node->this_word,read_word) == 0){       //does the word exist
                        found = 1;
                        add_payload(current->node->payload,queryId,dist);
                        //printf("I found word %s again\n",read_word);
                        break;
                    }
                    current = current->next;
                }
                if(found == -1){                // it was not found, we shall add it and add it to the list of buckets

                    bk_index node = NULL;
                    bk_add_node_hamming(&hamming_root_table[len-1], read_word,len, &node,queryId,dist);
                    //create this hash_bucket to store data
                    Hash_Bucket* new_bucket = malloc(sizeof(Hash_Bucket));
                    new_bucket->node = node;
                    new_bucket->next = NULL;

                    //append it to the head
                    Hash_Bucket* previous_first = hash_tables_hamming[len-1]->hash_buckets[word_hash_value];
                    //first bucket it now the new one
                    hash_tables_hamming[len-1]->hash_buckets[word_hash_value] = new_bucket;
                    new_bucket->next = previous_first;


                }
            }
        }
   


        //processing of current word ended, move on to the next one
        read_word = strtok(NULL, " ");
    }

}

//----------------------------------------Sigmod functions


ErrorCode InitializeIndex(){


    for(int i = 0; i<BLOOM_FILTER_SIZE; i++){
        bloom_filter_exact[i] = 0;
    }

    Q_Hash_Table = malloc(sizeof(Query_Hash_Table));
    Q_Hash_Table->query_hash_buckets = malloc(sizeof(Query*)*QUERY_HASH_BUCKETS);
    for(int i=0; i<= QUERY_HASH_BUCKETS-1; i++){
        Q_Hash_Table->query_hash_buckets[i] = NULL;
    }

    //Exact matching structures
    hash_table_exact = malloc(sizeof(Hash_table_exact*)* 29);
    for(int i = 0; i <=28 ; i++){
       hash_table_exact[i] = NULL;
    }

    //Hamming distance structures
    hash_tables_hamming = malloc(sizeof(Hash_table*)* 29);
    for(int i = 0; i <=28 ; i++){
       hash_tables_hamming[i] = NULL;
    }

    hamming_root_table = malloc(sizeof(bk_index)*29);
    for(int i = 0; i <=28 ; i++){
       hamming_root_table[i] = NULL;
    }

    hash_tables_edit = malloc(sizeof(Hash_table));
    hash_tables_edit->hash_buckets = malloc(sizeof(Hash_Bucket*)*EDIT_HASH_BUCKETS);
    for(int i=0; i< EDIT_HASH_BUCKETS; i++){
        hash_tables_edit->hash_buckets[i] = NULL;
    }
    ix = NULL;

    words_hash_table = malloc(sizeof(Words_Hash_Table));
    for(int i=0; i< WORD_HASH_TABLE_BUCKETS; i++){
        words_hash_table->words_hash_buckets[i] = NULL;
    }

	batch_results_list = malloc(sizeof(Batch_results_list));
	batch_results_list->head = NULL;
	batch_results_list->tail = NULL;

    return EC_SUCCESS;
}


//adds results of a doc in a list
void add_batch_result(int doc_id, int num_res, query_ids* results){
	Batch_results* new_node = malloc(sizeof(Batch_results));
	new_node->doc_id = doc_id;
	new_node->num_res = num_res;
	new_node->results = results;
	new_node->next = NULL;


	if(batch_results_list->head == NULL){
		batch_results_list->head = new_node;
		batch_results_list->tail = new_node;
	}else{
		batch_results_list->tail->next = new_node;
		batch_results_list->tail = new_node;
	}

}



ErrorCode MatchDocument(DocID doc_id, const char* doc_str){
    last_doc_id = doc_id;
    const word* read_word;
    char* temp = (char*)doc_str; 
    read_word = strtok(temp, " ");

    int docWordLen;

	query_ids* queries_head = NULL;
	int results_found = 0;



    //free result lists from previous doc
    while(r_node != NULL){
        result_node* temps = r_node; 
        r_node = r_node->next;
        free(temps);
    }

    while(r_node_bk_edit != NULL){
        result_node_bk* temps = r_node_bk_edit; 
        r_node_bk_edit = r_node_bk_edit->next;
        free(temps);
    }

    while(r_node_bk_hamming != NULL){
        result_node_bk* temps = r_node_bk_hamming; 
        r_node_bk_hamming = r_node_bk_hamming->next;
        free(temps);
    }

    
    r_node= NULL;
    r_node_bk_hamming = NULL;
    r_node_bk_edit = NULL;


    //for every word in document
    while(read_word != NULL){
        docWordLen = strlen(read_word);
		//printf("%s\n",read_word);
        //document deduplication here
        //hash this word to see if it exists
        int hash = djb2(read_word) % WORD_HASH_TABLE_BUCKETS;
        if(words_hash_table->words_hash_buckets[hash] != NULL){
            //bucket exists, check here
            int found = -1;
            Words_Hash_Bucket* current = words_hash_table->words_hash_buckets[hash];
            Words_Hash_Bucket* previous = NULL;
            while(current != NULL){//printf("comparing %s with %s",current->this_word,read_word);
                if(strcmp(current->this_word,read_word) == 0){
                    //word exists, skip
                    found = 1;//printf("Found word %s\n",read_word);
                    break;
                }
                previous = current;
                current = current->next;
            }
            if(found == 1){
                //word exists, skip
                read_word = strtok(NULL, " ");
                continue;                
            }else{
                //add it to the hash buckets, search it in structs
                previous->next = malloc(sizeof(Words_Hash_Bucket));
                strcpy(previous->next->this_word,read_word);
                previous->next->next = NULL;
            }
        }else{
            //create first bucket
            words_hash_table->words_hash_buckets[hash] = malloc(sizeof(Words_Hash_Bucket));
            strcpy(words_hash_table->words_hash_buckets[hash]->this_word,read_word);
            words_hash_table->words_hash_buckets[hash]->next = NULL ;
        }

        //look in edit distance
        for(int i=1;i<=3;i++){
            lookup_entry_index_edit(read_word,docWordLen,&ix,i);
        }
        //look in hamming distance
        if(hamming_root_table[docWordLen-1] != NULL){
            for(int i=1;i<=3;i++){
                lookup_entry_index_hamming(read_word,docWordLen,&hamming_root_table[docWordLen-1],i);
            }
        }

            
        //look in exact matching

        if(bloom_filter_exact[jenkins(read_word)%BLOOM_FILTER_SIZE] == 1 && bloom_filter_exact[djb2(read_word)%BLOOM_FILTER_SIZE] == 1){
            lookup_exact(read_word,hash_table_exact);
        }

        read_word = strtok(NULL, " ");
    }

   //printf("RESULTS FROM EXACT: \n\n");
    result_node* temp_node1 = r_node;
    while(temp_node1 != NULL){

        //printf("%s ->",temp_node1->this_entry->this_word);
        Payload* temp_payload = temp_node1->this_entry->payload;
        while(temp_payload != NULL){
            //before checking payload, check that this query hasnt been matched yet
            query_ids* temp_queries_head = queries_head;
            int matched = 0;
            while(temp_queries_head != NULL){
                if(temp_queries_head->queryID == temp_payload->queryId){
                    matched = 1;
                }
                temp_queries_head = temp_queries_head->next;
            }
            if(matched == 0){
            //printf("q:%d t:%d\n\n",temp_payload->queryId,temp_payload->threshold);
            Query* bucket = Q_Hash_Table->query_hash_buckets[temp_payload->queryId%QUERY_HASH_BUCKETS];
            while(bucket != NULL){
                if(bucket->query_id == temp_payload->queryId){      //query has been found
                    int to_find = bucket->query_words;              //we have to find this many words
                    for(int i=0; i<bucket->query_words; i++){
                        result_node* temp_list = r_node;
                        while(temp_list != NULL){
                            if(strcmp(bucket->words[i],temp_list->this_entry->this_word) == 0){
                                to_find--;
                                break;
                            }
                            temp_list = temp_list->next;
                        }
                    }
                    if(to_find == 0){
                        //printf("Query %d fully matched\n", temp_payload->queryId);
                        results_found++;
                        if(queries_head == NULL){       //first query, insert it at head
                            queries_head = malloc(sizeof(query_ids));
                            queries_head->next = NULL;
                            queries_head->queryID = temp_payload->queryId;
                        }else{                          //find correct position to sort
                            query_ids* temp = queries_head;
                            query_ids* previous;
                            int inserted = -1;
                            while(temp != NULL){
                                if(temp_payload->queryId < temp->queryID){      //insert it here
                                    
                                    if(temp == queries_head){
                                        query_ids* new_node = malloc(sizeof(query_ids));
                                        new_node->queryID = temp_payload->queryId;

                                        query_ids* previous_head = queries_head;
                                        queries_head = new_node;
                                        new_node->next = previous_head;
                                        inserted = 1;
                                        break;
                                    }else{

                                        query_ids* new_node = malloc(sizeof(query_ids));
                                    
                                        new_node->queryID = temp_payload->queryId;
                                        
                                        query_ids* previous_next = previous->next;
                                        
                                        previous->next = new_node;
                                        new_node->next = previous_next;
                                        inserted = 1;
                                        break;                                        
                                    }
                                }
                                previous = temp;
                                temp = temp->next;
                                //continue
                            }
                            //iterate through and is bigger than everything
                            //add to the end
                            if(inserted == -1){
                                temp = malloc(sizeof(query_ids));
                                temp->next = NULL;
                                temp->queryID = temp_payload->queryId;
                                previous->next = temp;
                            }
                        }
                    }
                }
                bucket = bucket->next;
            }
            }
            temp_payload = temp_payload->next;
        }
        //printf("\n");
        temp_node1 = temp_node1->next;
    }
    



    //printf("\n\n\n\n");
   //printf("RESULTS FROM Edit: \n\n");
    result_node_bk* temp_node2 = r_node_bk_edit;
    while(temp_node2 != NULL){

        Payload* temp_payload = temp_node2->this_entry->payload;
        // if(doc_id == 780){
        //     printf("%s thres: %d ->\n",temp_node2->this_entry->this_word,temp_node2->my_threshold);
        // }
        while(temp_payload != NULL){
        // if(doc_id == 780){
        //     printf("%d : %d\n",temp_payload->queryId,temp_payload->threshold );
        // }
            //before checking payload, check that this query hasnt been matched yet
            query_ids* temp_queries_head = queries_head;
            int matched = 0;
            while(temp_queries_head != NULL){
                if(temp_queries_head->queryID == temp_payload->queryId){
                    matched = 1;
                }
                temp_queries_head = temp_queries_head->next;
            }
            if(matched == 0){

            //printf("q:%d t:%d\n\n",temp_payload->queryId,temp_payload->threshold);
            Query* bucket = Q_Hash_Table->query_hash_buckets[temp_payload->queryId%QUERY_HASH_BUCKETS];
            while(bucket != NULL){
                if(bucket->query_id == temp_payload->queryId){      //query has been found
                    int to_find = bucket->query_words;              //we have to find this many words
                    //printf(" %d ",to_find);
                    for(int i=0; i<bucket->query_words; i++){
                        result_node_bk* temp_list = r_node_bk_edit;
                        while(temp_list != NULL){
                            if(strcmp(bucket->words[i],temp_list->this_entry->this_word) == 0 && bucket->match_dist >= temp_list->my_threshold){
                                to_find--;
                                break;
                            }
                            temp_list = temp_list->next;
                        }
                    }
                    if(to_find == 0){
                        //printf("Query %d fully matched\n", temp_payload->queryId);
                        results_found++;
                        if(queries_head == NULL){       //first query, insert it at head
                            queries_head = malloc(sizeof(query_ids));
                            queries_head->next = NULL;
                            queries_head->queryID = temp_payload->queryId;
                        }else{                          //find correct position to sort
                            query_ids* temp = queries_head;
                            query_ids* previous;
                            int inserted = -1;
                            while(temp != NULL){
                                if(temp_payload->queryId < temp->queryID){      //insert it here
                                    
                                    if(temp == queries_head){
                                        query_ids* new_node = malloc(sizeof(query_ids));
                                        new_node->queryID = temp_payload->queryId;

                                        query_ids* previous_head = queries_head;
                                        queries_head = new_node;
                                        new_node->next = previous_head;
                                        inserted = 1;
                                        break;
                                    }else{
                                        query_ids* new_node = malloc(sizeof(query_ids));
                                    
                                        new_node->queryID = temp_payload->queryId;
                                        
                                        query_ids* previous_next = previous->next;
                                        
                                        previous->next = new_node;
                                        new_node->next = previous_next;
                                        inserted = 1;
                                        break;                                        
                                    }
                                }
                                previous = temp;
                                temp = temp->next;
                                //continue
                            }
                            //iterate through and is bigger than everything
                            //add to the end
                            if(inserted == -1){
                                temp = malloc(sizeof(query_ids));
                                temp->next = NULL;
                                temp->queryID = temp_payload->queryId;
                                previous->next = temp;
                            }
                        }
                    }
                    
                }
                bucket = bucket->next;
            }
            }
            temp_payload = temp_payload->next;
        }
        temp_node2 = temp_node2->next;
    }
    
    //printf("RESULTS FROM hamming: \n\n");
    result_node_bk* temp_node3 = r_node_bk_hamming;
    while(temp_node3 != NULL){
        //printf("%s ->",temp_node3->this_entry->this_word);
        Payload* temp_payload = temp_node3->this_entry->payload;
        while(temp_payload != NULL){
            //before checking payload, check that this query hasnt been matched yet
            query_ids* temp_queries_head = queries_head;
            int matched = 0;
            while(temp_queries_head != NULL){
                if(temp_queries_head->queryID == temp_payload->queryId){
                    matched = 1;
                }
                temp_queries_head = temp_queries_head->next;
            }
            if(matched == 0){//printf("q:%d t:%d\n\n",temp_payload->queryId,temp_payload->threshold);

            Query* bucket = Q_Hash_Table->query_hash_buckets[temp_payload->queryId%QUERY_HASH_BUCKETS];
            while(bucket != NULL){
                if(bucket->query_id == temp_payload->queryId){      //query has been found
                    int to_find = bucket->query_words;              //we have to find this many words
                    for(int i=0; i<bucket->query_words; i++){
                        result_node_bk* temp_list = r_node_bk_hamming;
                        while(temp_list != NULL){
                            if(strcmp(bucket->words[i],temp_list->this_entry->this_word) == 0){
                                to_find--;
                                break;
                            }
                            temp_list = temp_list->next;
                        }
                    }
                    if(to_find == 0){
                        //printf("Query %d fully matched\n", temp_payload->queryId);
                        results_found++;
                        if(queries_head == NULL){       //first query, insert it at head
                            queries_head = malloc(sizeof(query_ids));
                            queries_head->next = NULL;
                            queries_head->queryID = temp_payload->queryId;
                        }else{                          //find correct position to sort
                            query_ids* temp = queries_head;
                            query_ids* previous;
                            int inserted = -1;
                            while(temp != NULL){
                                if(temp_payload->queryId < temp->queryID){      //insert it here
                                    
                                    if(temp == queries_head){
                                        query_ids* new_node = malloc(sizeof(query_ids));
                                        new_node->queryID = temp_payload->queryId;

                                        query_ids* previous_head = queries_head;
                                        queries_head = new_node;
                                        new_node->next = previous_head;
                                        inserted = 1;
                                        break;
                                    }else{
                                        query_ids* new_node = malloc(sizeof(query_ids));
                                    
                                        new_node->queryID = temp_payload->queryId;
                                        
                                        query_ids* previous_next = previous->next;
                                        
                                        previous->next = new_node;
                                        new_node->next = previous_next;
                                        inserted = 1;
                                        break;                                        
                                    }
                                }
                                previous = temp;
                                temp = temp->next;
                                //continue
                            }
                            //iterate through and is bigger than everything
                            //add to the end
                            if(inserted == -1){
                                temp = malloc(sizeof(query_ids));
                                temp->next = NULL;
                                temp->queryID = temp_payload->queryId;
                                previous->next = temp;
                            }
                        }
                    
                    }
                }
                bucket = bucket->next;
            }
            }
            temp_payload = temp_payload->next;
        }
        //printf("\n");
        temp_node3 = temp_node3->next;
    }

	//add the results of a doc in results list
	add_batch_result(doc_id,results_found,queries_head);

    reset_words_hash_table();
	return EC_SUCCESS;
}

ErrorCode StartQuery(QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist){

    //add the query to the query hash table
    add_query(query_id%QUERY_HASH_BUCKETS,query_id,query_str, match_type, match_dist);

    //update appropriate data structures
    if(match_type == 0){
        deduplicate_exact_matching(query_str, query_id, match_dist, match_type);
    }else if(match_type == 1){
        deduplicate_hamming(query_str, query_id, match_dist, match_type);
    }else if(match_type == 2){
        deduplicate_edit_distance(query_str, query_id, match_dist, match_type, &ix);
    }



}

ErrorCode EndQuery(QueryID query_id){
	// Remove this query from the active query set

    Query* bucket = Q_Hash_Table->query_hash_buckets[query_id%QUERY_HASH_BUCKETS];
    //we are freeing the first bucket
    if(bucket != NULL && bucket->query_id == query_id){
        Query* temp = Q_Hash_Table->query_hash_buckets[query_id%QUERY_HASH_BUCKETS];
        Q_Hash_Table->query_hash_buckets[query_id%QUERY_HASH_BUCKETS] = Q_Hash_Table->query_hash_buckets[query_id%QUERY_HASH_BUCKETS]->next;
        free(temp);
        return EC_SUCCESS;
    }

    Query* bucket_prev = bucket;
    while(bucket != NULL){
        if(bucket->query_id == query_id){
            bucket_prev->next = bucket->next;
            free(bucket);
            return EC_SUCCESS;
        }
        bucket_prev = bucket;
        bucket = bucket->next;
    }
	return EC_SUCCESS;
}



//adds query to query hashtabel
ErrorCode add_query(int bucket_num, QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist){
    if(Q_Hash_Table->query_hash_buckets[bucket_num] == NULL){
        //initialize bucket
        Q_Hash_Table->query_hash_buckets[bucket_num] = malloc(sizeof(Query));

        //store data
        Q_Hash_Table->query_hash_buckets[bucket_num]->query_id = query_id;
        strcpy(Q_Hash_Table->query_hash_buckets[bucket_num]->str,query_str);
        
        char temp_query_str[MAX_QUERY_LENGTH];
        strcpy(temp_query_str,query_str);
        char* read_word = strtok(temp_query_str, " ");
        int num = 0;
        while(read_word != NULL){
            strcpy(Q_Hash_Table->query_hash_buckets[bucket_num]->words[num],read_word);
            num++;
            read_word = strtok(NULL, " ");
        }
        Q_Hash_Table->query_hash_buckets[bucket_num]->query_words = num;
        Q_Hash_Table->query_hash_buckets[bucket_num]->match_dist = match_dist;
        Q_Hash_Table->query_hash_buckets[bucket_num]->match_type = match_type;

        Q_Hash_Table->query_hash_buckets[bucket_num]->next = NULL;
    }else{
        //create new bucket
        //add it to the start, save time in parsing till the end
        
        Query* bucket = malloc(sizeof(Query));

        //store data
        bucket->query_id = query_id;
        strcpy(bucket->str,query_str);
        bucket->match_dist = match_dist;
        bucket->match_type = match_type;

        char temp_query_str[MAX_QUERY_LENGTH];
        strcpy(temp_query_str,query_str); 
        char* read_word = strtok(temp_query_str, " ");
        int num = 0;
        while(read_word != NULL){
            strcpy(bucket->words[num],read_word);
            num++;
            read_word = strtok(NULL, " ");
        }
        bucket->query_words = num;

        //swap first and new
        Query* temp = Q_Hash_Table->query_hash_buckets[bucket_num];
        Q_Hash_Table->query_hash_buckets[bucket_num] = bucket;
        bucket->next = temp;
    }

}



//--------------------------------------------destructors

ErrorCode DestroyIndex(){

    delete_hash_tables_edit();
    delete_hash_tables_hamming();
    delete_hash_tables_exact();

    for(int i=0; i<= QUERY_HASH_BUCKETS-1; i++){
        if( Q_Hash_Table->query_hash_buckets[i] != NULL){
            //free all buckets from the list here
            Query* bucket = Q_Hash_Table->query_hash_buckets[i];
            while(Q_Hash_Table->query_hash_buckets[i] != NULL){
                bucket = Q_Hash_Table->query_hash_buckets[i];
                Q_Hash_Table->query_hash_buckets[i] =  Q_Hash_Table->query_hash_buckets[i]->next;
                free(bucket);
            }
        }
    }
    free(Q_Hash_Table->query_hash_buckets);
    free(Q_Hash_Table);

    //free result lists
    while(r_node != NULL){
        result_node* temp = r_node; 
        r_node = r_node->next;
        free(temp);
    }

    while(r_node_bk_edit != NULL){
        result_node_bk* temp = r_node_bk_edit; 
        r_node_bk_edit = r_node_bk_edit->next;
        free(temp);
    }

    while(r_node_bk_hamming != NULL){
        result_node_bk* temp = r_node_bk_hamming; 
        r_node_bk_hamming = r_node_bk_hamming->next;
        free(temp);
    }

    // query_ids* temp = queries_head;
    // while(queries_head != NULL){
    //     temp = queries_head;
    //     queries_head = queries_head->next;
    //     free(temp);
    // }

	//preeeeeeeeeepei na gieni destroy sto results list

    free_words_hash_table();
}

void delete_hash_tables_edit(){

        //if this hash table has been allocated, free it; else move on
        if(hash_tables_edit != NULL){
            for(int j=0; j < EDIT_HASH_BUCKETS; j++){
                //Hash_Bucket* current = hash_tables[i]->hash_buckets[j];
                    Hash_Bucket* temp ; 
                    while(hash_tables_edit->hash_buckets[j] != NULL){        //if there exists at least one bucket, free it and every next it has
                        temp = hash_tables_edit->hash_buckets[j];
                        hash_tables_edit->hash_buckets[j] = hash_tables_edit->hash_buckets[j]->next;
                        free(temp); 
                    }
            }
            free(hash_tables_edit->hash_buckets);
            free(hash_tables_edit);
        }

    destroy_entry_index(&ix);
}

void delete_hash_tables_hamming(){
    
    for(int i=0; i <= 28; i++){
        //if this hash table has been allocated, free it; else move on
        if(hash_tables_hamming[i] != NULL){
            for(int j=0; j < HAMMING_HASH_BUCKET; j++){
                //free the hash_table itself
                Hash_Bucket* temp ; 
                while(hash_tables_hamming[i]->hash_buckets[j] != NULL){        //if there exists at least one bucket, free it and every next it has
                    temp = hash_tables_hamming[i]->hash_buckets[j];
                    hash_tables_hamming[i]->hash_buckets[j] = hash_tables_hamming[i]->hash_buckets[j]->next;
                    free(temp); 
                }
            }
            free(hash_tables_hamming[i]->hash_buckets);
            free(hash_tables_hamming[i]);

            //if hash table has been initialized then we have at least a root for a bk of this length
            //free the bk trees starting from their roots
            destroy_entry_index(&hamming_root_table[i]);
        }
    }

    //finally, free both arrays
    free(hash_tables_hamming);
    free(hamming_root_table);
}

void delete_hash_tables_exact(){
    for(int i=0; i <= 28; i++){
        //if this hash table has been allocated, free it; else move on
        if(hash_table_exact[i] != NULL){
            for(int j=0; j < EXACT_HASH_BUCKET; j++){
                //free the hash_table itself
                entry temp ; 
                while(hash_table_exact[i]->hash_buckets[j] != NULL){        //if there exists at least one bucket, free it and every next it has
                    temp = hash_table_exact[i]->hash_buckets[j];
                    hash_table_exact[i]->hash_buckets[j] = hash_table_exact[i]->hash_buckets[j]->next;
                    destroy_entry(&temp); 
                }
            }
            free(hash_table_exact[i]->hash_buckets);
            free(hash_table_exact[i]);
        }
    }
    free(hash_table_exact);
}

void free_words_hash_table(){
    for(int i=0; i< WORD_HASH_TABLE_BUCKETS; i++){
        while(words_hash_table->words_hash_buckets[i] != NULL){
            Words_Hash_Bucket* temp = words_hash_table->words_hash_buckets[i];
            words_hash_table->words_hash_buckets[i] = words_hash_table->words_hash_buckets[i]->next;
            free(words_hash_table->words_hash_buckets[i]);
        }
    }
    free(words_hash_table);
    words_hash_table = NULL;
}

void reset_words_hash_table(){
    //free it
    for(int i=0; i< WORD_HASH_TABLE_BUCKETS; i++){
        while(words_hash_table->words_hash_buckets[i] != NULL){
            Words_Hash_Bucket* temp = words_hash_table->words_hash_buckets[i];
            words_hash_table->words_hash_buckets[i] = words_hash_table->words_hash_buckets[i]->next;
            free(temp);
        }
    }
    free(words_hash_table);
    words_hash_table = NULL;
    
    //create the new one
    words_hash_table = malloc(sizeof(Words_Hash_Table));
    for(int i=0; i< WORD_HASH_TABLE_BUCKETS; i++){
        words_hash_table->words_hash_buckets[i] = NULL;
    }    
}


//----------------------------------print functions used for debugging perposes


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

void print_query_hash_buckets(){
    for(int i=0; i<= QUERY_HASH_BUCKETS-1 ; i++){
        if(Q_Hash_Table->query_hash_buckets[i] == NULL){
            printf("buckets for  %d are null\n",i);
        }
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
                    while(temp != NULL){
                        printf("%s     ->\n\n",temp->this_word);
                        Payload* temp_payload = temp->payload;
                        while(temp_payload != NULL){
                            printf("q:%d t:%d\n\n",temp_payload->queryId,temp_payload->threshold);
                            temp_payload = temp_payload->next;
                        }
                        temp =temp->next;
                    }
                }
            }
        }
    }
}

void print_query_list(){
    int count = 0;
    for(int i=0; i<=9; i++){
        printf("Printing Bucket %d\n",i);
        if(Q_Hash_Table->query_hash_buckets[i] != NULL ){
            Query* current = Q_Hash_Table->query_hash_buckets[i];
            while(current != NULL){
                printf("%d ->",current->query_id);
                count++;
                current = current->next;
            }
        }
        printf("\n");
    }
    printf("I have stored %d queries total\n",count);
}

ErrorCode GetNextAvailRes(DocID* p_doc_id, unsigned int* p_num_res, QueryID** p_query_ids){
    	
	*p_doc_id = batch_results_list->head->doc_id;
    *p_num_res = batch_results_list->head->num_res;
    *p_query_ids = (int*)malloc(sizeof(int)*(*p_num_res));
    query_ids* temp = batch_results_list->head->results;
    for(int i=0; i<(*p_num_res); i++){
        (*p_query_ids)[i] = temp->queryID;
        temp = temp->next;
    }
	

	temp = batch_results_list->head->results;
	Batch_results* previous = batch_results_list->head ;
	batch_results_list->head = batch_results_list->head->next;

	//destoy result of p_doc_id before given to main
	while(temp != NULL){
		query_ids* temp2 = temp;
		temp = temp->next;
		free(temp2);
	}
	free(previous);

	return EC_SUCCESS;
}




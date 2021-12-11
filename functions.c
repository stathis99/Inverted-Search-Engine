#include "./include/structs.h"
#include <stdlib.h>
#include <time.h> 
#include <string.h>

//global structs we are using
int bloom_filter_exact[BLOOM_FILTER_SIZE];

Query_Hash_Table* Q_Hash_Table;

bk_index ix;
//Hash_table** hash_tables_edit;
Hash_table* hash_tables_edit;

bk_index* hamming_root_table;
Hash_table** hash_tables_hamming;

Hash_table_exact** hash_tables_exact;

result_node* r_node= NULL;
result_node_bk* r_node_bk_hamming = NULL;
result_node_bk* r_node_bk_edit = NULL;

query_ids* queries_head = NULL;
double time_spent = 0.0;

//entry functions
void free_word(word* w){
    free(w);
}

static int distance (const char * word1,
                     int len1,
                     const char * word2,
                     int len2)
{
    //clock_t begin = clock();


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



enum error_code create_entry(const word* w, entry* e,unsigned int queryId,int dist){
    *e = malloc(sizeof(Entry));

    (*e)->this_word = (word*)w;

    (*e)->payload = malloc(sizeof(Payload));
    (*e)->payload->queryId = queryId;
    (*e)->payload->threshold = dist;
    (*e)->payload->next = NULL;

    (*e)->next = NULL;
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
        //clock added

    //clock_t begin = clock();

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
    //clock_t end = clock();
    //time_spent += (double)(end - begin) / CLOCKS_PER_SEC;

    return cost[len1][len2];
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


//bk functions
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

//creates a bk tree node
bk_index bk_create_node(bk_index* ix,word* entry_word,int weight,int queryId, int dist){

        (*ix) = malloc(sizeof(Index));
        (*ix)->weight = weight;
        (*ix)->this_word = entry_word;
        (*ix)->next = NULL;
        (*ix)-> child = NULL;

        //this is for payload might change
        (*ix)->payload = malloc(sizeof(Payload));
        (*ix)->payload->queryId = queryId;
        (*ix)->payload->threshold = dist;
        (*ix)->payload->next = NULL;

        return *ix;
}


enum error_code look_for_threshold(struct Payload* payload,int threshold,const word* q_w,const word* w,int match_type ,bk_index temp){
    while(payload != NULL){
        if(payload->threshold == threshold){
            //printf("%s = %s q:%d t:%d \n",q_w,w,payload->queryId,payload->threshold);
            if(match_type == 1){            //we are using edit
                if (strcmp(temp->this_word,q_w) == 0){
                            //printf("%s     ->\n\n",temp->this_word);
                            if(r_node_bk_edit == NULL){
                                r_node_bk_edit = malloc(sizeof(result_node_bk));
                                r_node_bk_edit->next = NULL;
                                r_node_bk_edit->this_entry = temp;                         
                            }else{
                                result_node_bk* current = r_node_bk_edit;
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

                                    result_node_bk* second = r_node_bk_edit->next;
                                    r_node_bk_edit->next = new_node;
                                    new_node->next = second;                
                                }
                            }                         
                        }
            }else if(match_type == 2){          //we are using hamming
                if (strcmp(temp->this_word,q_w) == 0){
                            //printf("%s     ->\n\n",temp->this_word);
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
        }
        payload = payload->next;
    }
}


enum error_code lookup_entry_index(const word* w, bk_index* ix, int threshold,int match_type){
    bk_index temp_child  = (*ix)->child;
    int dist;
    if(match_type == 1){
        dist = distance(w,strlen(w),(*ix)->this_word,strlen((*ix)->this_word));
    }else{
        dist = hamming_distance(w,(*ix)->this_word,strlen(w));
    }
    int min_dist = dist - threshold;
    int max_dist = dist + threshold;

    //if ix keyword is close to w then create an entry and add it to the results
    if( dist <= threshold ){
        look_for_threshold((*ix)->payload,threshold,(*ix)->this_word,w,match_type,*ix);
    }

    //traverse the tree retrospectively 
    while(temp_child != NULL){
        //if distance child from the parent is  [𝑑 − 𝑛, 𝑑 + 𝑛] then call retrospectively 
        if(temp_child->weight >= min_dist && temp_child->weight <= max_dist ){
            lookup_entry_index(w,&temp_child,threshold,match_type);
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
        //lookup_entry_index(head->this_word,ix,threshold);
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
        dist = distance(entry_word,strlen(entry_word),(*ix)->this_word,strlen((*ix)->this_word));
    }else if(type == HAMMING_DIST){
        dist = hamming_distance(entry_word,(*ix)->this_word,strlen((*ix)->this_word));
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
                bk_add_node(&temp_child ,entry_word,type, node,queryId,threshold);
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
                 word* my_word = malloc(len+1);
                 strcpy(my_word,read_word);

                 bk_create_node(ix,my_word,0,queryId,dist);
                 hash_tables_edit->hash_buckets[word_hash_value]->node = *ix;

            }else{
                word* my_word = malloc(len+1);
                strcpy(my_word,read_word);

                bk_index node = NULL;
                bk_add_node(ix, my_word, 1, &node, queryId,dist);
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
                word* my_word = malloc(len+1);
                strcpy(my_word,read_word);

                bk_index node = NULL;
                bk_add_node(ix, my_word, 1, &node,queryId,dist);
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

/*void deduplicate_edit_distance2(const char* temp, unsigned int queryId, int dist, int type, Hash_table** hash_table, bk_index* ix){
    char* read_word;
    char* temp_temp = (char*)temp; 
    read_word = strtok(temp_temp, " ");
    while(read_word != NULL){
        int len = strlen(read_word);
        //printf("%s %d %d %d \n",read_word ,queryId, dist, type );

        int word_hash_value = hash(read_word)%10;
        //printf("Word %s hashes to %d\n",read_word,word_hash_value);
        if(hash_table == NULL){

            //initialize hashtable 
        hash_table = malloc(sizeof(Hash_table));    
            hash_table->hash_buckets = malloc(sizeof(Hash_Bucket*)*10);
            
            //initialize hash buckets
            for(int i=0 ; i<= 9 ;i++){
                hash_table[len-1]->hash_buckets[i] = NULL;
            }

            //hash word to find bucket
            hash_table[len-1]->hash_buckets[word_hash_value] = malloc(sizeof(Hash_Bucket));

            if((*ix) == NULL){                  //bk root hasnt been initialized
                word* my_word= malloc(len + 1);
                strcpy(my_word,read_word);

                bk_create_node(ix,my_word,0,queryId,dist);
                hash_table[len-1]->hash_buckets[word_hash_value]->node = *ix;
                hash_table[len-1]->hash_buckets[word_hash_value]->next = NULL;

            }else{
                word* my_word= malloc(len + 1);
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

                word* my_word= malloc(len + 1);
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
                    word* my_word = malloc(len + 1);
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

}*/

unsigned long hash(unsigned char *str){
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c;

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



void deduplicate_exact_matching(const char* temp, unsigned int queryId, int dist, int type, Hash_table_exact** hash_table_exact){
    
    char* read_word;
    char* temp_temp = (char*)temp; 
    read_word = strtok(temp_temp, " ");
    while(read_word != NULL){
        int len = strlen(read_word);
        //printf("%s %d %d %d \n",read_word ,queryId, dist, type );

        bloom_filter_exact[djb2(read_word)%BLOOM_FILTER_SIZE] = 1;
        bloom_filter_exact[jenkins(read_word)%BLOOM_FILTER_SIZE] = 1;


        int word_hash_value = hash(read_word)%10;
        //printf("Word %s hashes to j:%d dj:%d\n",read_word,word_hash_value1,word_hash_value2);
        
        if(hash_table_exact[len-1] == NULL){

            //initialize hashtable 
            hash_table_exact[len-1] = malloc(sizeof(Hash_table_exact));
            hash_table_exact[len-1]->hash_buckets = malloc(sizeof(entry)*10);

            //initialize hash buckets
            for(int i=0 ; i<= 9 ;i++){
                hash_table_exact[len-1]->hash_buckets[i] = NULL;
            }
            word* my_word= malloc(len + 1);
            strcpy(my_word,read_word);

            create_entry(my_word,&hash_table_exact[len-1]->hash_buckets[word_hash_value],queryId,dist);
            //free_word(my_word);

        }else{              //this hash table has been initialized
        
            entry current = hash_table_exact[len-1]->hash_buckets[word_hash_value];
            if(hash_table_exact[len-1]->hash_buckets[word_hash_value] == NULL){

                word* my_word= malloc(len + 1);
                strcpy(my_word,read_word);

                create_entry(my_word,&hash_table_exact[len-1]->hash_buckets[word_hash_value],queryId,dist);
                //free_word(my_word);
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
                    word* my_word= malloc(len + 1);
                    strcpy(my_word,read_word);

                    entry temp_entry = NULL;

                    create_entry(my_word,&temp_entry,queryId,dist);
                    add_entry_no_list(hash_table_exact[len-1]->hash_buckets[word_hash_value],temp_entry);
                    //free_word(my_word);
                }
            }
        }
        //processing of current word ended, move on to the next one
        read_word = strtok(NULL, " ");
    }

}

void deduplicate_hamming(const char* temp, unsigned int queryId, int dist, int type, Hash_table** hash_table, bk_index* hamming_root_table){


    char* read_word;
    char* temp_temp = (char*)temp; 
    read_word = strtok(temp_temp, " ");
    while(read_word != NULL){
        int len = strlen(read_word);
        //printf("%s %d %d %d \n",read_word ,queryId, dist, type );

        int word_hash_value = hash(read_word)%10;
        //printf("Word %s hashes to %d\n",read_word,word_hash_value);
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

            if(hamming_root_table[len-1] == NULL){                  //bk root hasnt been initialized
                word* my_word= malloc(len + 1);
                strcpy(my_word,read_word);

                bk_create_node(&hamming_root_table[len-1],my_word,0,queryId,dist);
                hash_table[len-1]->hash_buckets[word_hash_value]->node = hamming_root_table[len-1];
                hash_table[len-1]->hash_buckets[word_hash_value]->next = NULL;
            }else{
                //unnecessary, just fix struct word
                word* my_word= malloc(len + 1);
                strcpy(my_word,read_word);

                bk_index node = NULL;
                bk_add_node(&hamming_root_table[len-1], my_word, 1, &node,queryId,dist);
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

                word* my_word= malloc(len + 1);
                strcpy(my_word,read_word);

                bk_index node = NULL;
                bk_add_node(&hamming_root_table[len-1], my_word, 2, &node,queryId,dist);
                hash_table[len-1]->hash_buckets[word_hash_value]->node = node;
                hash_table[len-1]->hash_buckets[word_hash_value]->next = NULL;
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
                    word* my_word= malloc(len + 1);
                    strcpy(my_word,read_word);

                    bk_index node = NULL;
                    bk_add_node(&hamming_root_table[len-1], my_word, 2, &node,queryId,dist);
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

void delete_hash_tables_hamming(){
    
    for(int i=0; i <= 28; i++){
        //if this hash table has been allocated, free it; else move on
        if(hash_tables_hamming[i] != NULL){
            for(int j=0; j <= 9; j++){
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

void lookup_exact(const word* w,Hash_table_exact** hash_table_exact){
    for(int i=0; i<=28; i++){
        if(hash_table_exact[i] != NULL){
            for(int j=0; j< 10; j++){
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



ErrorCode MatchDocument(DocID doc_id, const char* doc_str){

    
    const word* read_word;
    char* temp = (char*)doc_str; 
    read_word = strtok(temp, " ");
    int count=0;
    int counte=0;
    int counth=0;
    int countex=0;

    int querys_matched = 0;

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

    query_ids* temps = queries_head;
    while(queries_head != NULL){
        temps = queries_head;
        queries_head = queries_head->next;
        free(temps);
    }
    
    r_node= NULL;
    r_node_bk_hamming = NULL;
    r_node_bk_edit = NULL;

    queries_head = NULL;

    while(read_word != NULL){

        //look in edit distance
        for(int i=1;i<=3;i++){
            lookup_entry_index(read_word,&ix,i,1);
        }
        //look in hamming distance
            int length_word = strlen(read_word)-1;
                if(hamming_root_table[length_word] != NULL){
                    for(int i=1;i<=3;i++){
                       lookup_entry_index(read_word,&hamming_root_table[length_word],i,2);
                    }
                }

            
        //look in exact matching

        if(bloom_filter_exact[jenkins(read_word)%BLOOM_FILTER_SIZE] == 1 && bloom_filter_exact[djb2(read_word)%BLOOM_FILTER_SIZE] == 1){
            lookup_exact(read_word,hash_tables_exact);
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
                        querys_matched;
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
    
   //printf("RESULTS FROM Edit: \n\n");
    result_node_bk* temp_node2 = r_node_bk_edit;
    while(temp_node2 != NULL){
        //printf("%s ->",temp_node2->this_entry->this_word);
        Payload* temp_payload = temp_node2->this_entry->payload;
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
                    //printf(" %d ",to_find);
                    for(int i=0; i<bucket->query_words; i++){
                        result_node_bk* temp_list = r_node_bk_edit;
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
                        querys_matched;
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
                        querys_matched;
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

//     //print sorted query_ids list
//     query_ids* current = queries_head;
//     while(current != NULL){
//         printf("%d ->",current->queryID);
//         current = current->next;
//     }
// printf("\n");
//print_bk_tree(ix,0);
}

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
    hash_tables_exact = malloc(sizeof(Hash_table_exact*)* 29);
    for(int i = 0; i <=28 ; i++){
       hash_tables_exact[i] = NULL;
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

    //Edit distance structures
    // hash_tables_edit = malloc(sizeof(Hash_table*)* 29);
    // for(int i = 0; i <=28 ; i++){
    //    hash_tables_edit[i] = NULL;
    // }
    hash_tables_edit = malloc(sizeof(Hash_table));
    hash_tables_edit->hash_buckets = malloc(sizeof(Hash_Bucket*)*EDIT_HASH_BUCKETS);
    for(int i=0; i< EDIT_HASH_BUCKETS; i++){
        hash_tables_edit->hash_buckets[i] = NULL;
    }
    ix = NULL;

    return EC_SUCCESS;
}

int strlen_my(const char* word){
    int len = 4;
    while(word[len] != '\0'){
        len++;
    }
    return len;
}

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

ErrorCode StartQuery (QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist){
    //printf(" %d \n",query_id);
    //add the query to the query hash table

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

    add_query(query_id%QUERY_HASH_BUCKETS,query_id,query_str, match_type, match_dist);

    //update appropriate data structures
    if(match_type == 0){
        deduplicate_exact_matching(query_str, query_id, match_dist, match_type, hash_tables_exact);
    }else if(match_type == 1){
        deduplicate_hamming(query_str, query_id, match_dist, match_type, hash_tables_hamming, hamming_root_table);
    }else if(match_type == 2){
        deduplicate_edit_distance(query_str, query_id, match_dist, match_type, &ix);
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

void match_query(result_node* head){

}

ErrorCode DestroyIndex(){

    printf("The time %f seconds\n", time_spent);



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

    query_ids* temp = queries_head;
    while(queries_head != NULL){
        temp = queries_head;
        queries_head = queries_head->next;
        free(temp);
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
// size_t strlen_my(const char *str)
// {
//     const char *cptr = str;
//     uint32_t i, *s;
//     size_t ctr = 0;

//     /* Satisfy alignment requirements */
//     while((uintptr_t)cptr & (sizeof(uint32_t)-1))
//     {
//         if(!*cptr)
//             return cptr - str;
//         cptr++;
//     }

//     s = (uint32_t *)cptr;

//     do {
//         i = s[ctr];
//         /* Mask off high bit */
//         i &= HIGH_MASK;
//         /* subtract 0x01 from each byte,
//            giving a set high bit if it
//            was zero */
//         i -= LOW_MASK;
//         ctr++;
//             /* Test for the set high bit
//                and if it is found exit */
//     } while(!(i &= NOT_HIGH_MASK));

//     /* Find the first high bit set and
//        create the corresponding byte index */
//     i = div8(ffs(i));
//     /* Remove the counter increment from the
//        loop and multiply it by 4 to find the
//        rest of the byte index */
//     ctr = mul4_32(ctr - 1);

//     /* Return the combined byte index with 1
//        removed so it doesn't include the null
//        terminator itself */
//     return (i + ctr - 1);
// }

uint32_t gatopeich_strlen32(const char* str)
{
    uint32_t *u32 = (uint32_t*)str, u, abcd, i=0;
    while(1)
    {
        u = u32[i++];
        abcd = (u-0x01010101) & 0x80808080;
        if (abcd && // If abcd is not 0, we have NUL or a non-ASCII char > 127...
             (abcd &= ~u)) // ... Discard non-ASCII chars
        {
        #if BYTE_ORDER == BIG_ENDIAN
            return 4*i - (abcd&0xffff0000 ? (abcd&0xff000000?4:3) : abcd&0xff00?2:1);
        #else
            return 4*i - (abcd&0xffff ? (abcd&0xff?4:3) : abcd&0xff0000?2:1);
        #endif
        }
    }
}
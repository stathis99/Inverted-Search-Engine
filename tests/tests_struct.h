//used for testing
#define QUERY_HASH_BUCKETS_TEST 4
#define EDIT_HASH_BUCKETS_TEST 100
#define WORD_HASH_TABLE_BUCKETS_TEST 100
#define EXACT_HASH_BUCKET_TEST 100
#define HAMMING_HASH_BUCKET_TEST 100

Hash_table* hash_tables_edit;

bk_index* hamming_root_table;
Hash_table** hash_tables_hamming;

Hash_table_exact** hash_table_exact;
Query_Hash_Table* Q_Hash_Table_Test;
Query_Hash_Table* Q_Hash_Table;
bk_index ix;
int bloom_filter_exact[10];

void InitializeIndex_QueryTest();
ErrorCode StartQuery_QueryTest(QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist);
ErrorCode add_query_Test(int bucket_num, QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist);
ErrorCode _Insert(QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist);
int hamming_dist(const char* str1, const char* str2, int len1, int len2);
ErrorCode InitializeIndex_Insert(){


    for(int i = 0; i<10; i++){
        bloom_filter_exact[i] = 0;
    }

    Q_Hash_Table = malloc(sizeof(Query_Hash_Table));
    Q_Hash_Table->query_hash_buckets = malloc(sizeof(Query*)*QUERY_HASH_BUCKETS_TEST);
    for(int i=0; i<= QUERY_HASH_BUCKETS_TEST-1; i++){
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
    hash_tables_edit->hash_buckets = malloc(sizeof(Hash_Bucket*)*EDIT_HASH_BUCKETS_TEST);
    for(int i=0; i< EDIT_HASH_BUCKETS_TEST; i++){
        hash_tables_edit->hash_buckets[i] = NULL;
    }
    ix = NULL;
    return EC_SUCCESS;
}

int distance (const char * word1,int len1, const char * word2,int len2){

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

enum error_code add_entry_no_list(entry first, const entry new_entry){
    if(first == NULL){
        return NULL_POINTER;
    }
    new_entry->next = first->next;
    first->next = new_entry;
    return SUCCESS;
}

int hamming_dist(const char* str1, const char* str2, int len1, int len2){
    if(len1 != len2){
        return -1;
    }
    //compare characters one by one
    int distance = 0;
    
    for(int i=0; i < len1; i++){
        if(str1[i] != str2[i]){
            distance++;
        }
    }
    return distance;   
}

//all these are used for unit testing
void InitializeIndex_QueryTest(){
    Q_Hash_Table_Test = malloc(sizeof(Query_Hash_Table));
    Q_Hash_Table_Test->query_hash_buckets = malloc(sizeof(Query*)*QUERY_HASH_BUCKETS_TEST);
    for(int i=0; i< QUERY_HASH_BUCKETS_TEST; i++){
        Q_Hash_Table_Test->query_hash_buckets[i] = NULL;
    }
    return;
}

ErrorCode StartQuery_QueryTest(QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist){
    add_query_Test(query_id%QUERY_HASH_BUCKETS_TEST,query_id,query_str, match_type, match_dist);
    return EC_SUCCESS;
}

ErrorCode DestroyIndex_QueryTest(){

    for(int i=0; i< QUERY_HASH_BUCKETS_TEST; i++){
        if( Q_Hash_Table_Test->query_hash_buckets[i] != NULL){
            //free all buckets from the list here
            Query* bucket = Q_Hash_Table_Test->query_hash_buckets[i];
            while(Q_Hash_Table_Test->query_hash_buckets[i] != NULL){
                bucket = Q_Hash_Table_Test->query_hash_buckets[i];
                Q_Hash_Table_Test->query_hash_buckets[i] =  Q_Hash_Table_Test->query_hash_buckets[i]->next;
                free(bucket);
            }
        }
    }
    free(Q_Hash_Table_Test->query_hash_buckets);
    free(Q_Hash_Table_Test);
    return EC_SUCCESS;
}

ErrorCode add_query_Test(int bucket_num, QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist){
    if(Q_Hash_Table_Test->query_hash_buckets[bucket_num] == NULL){
        //initialize bucket
        Q_Hash_Table_Test->query_hash_buckets[bucket_num] = malloc(sizeof(Query));

        //store data
        Q_Hash_Table_Test->query_hash_buckets[bucket_num]->query_id = query_id;
        strcpy(Q_Hash_Table_Test->query_hash_buckets[bucket_num]->str,query_str);
        
        char temp_query_str[MAX_QUERY_LENGTH];
        strcpy(temp_query_str,query_str);
        char* read_word = strtok(temp_query_str, " ");
        int num = 0;
        while(read_word != NULL){
            strcpy(Q_Hash_Table_Test->query_hash_buckets[bucket_num]->words[num],read_word);
            num++;
            read_word = strtok(NULL, " ");
        }
        Q_Hash_Table_Test->query_hash_buckets[bucket_num]->query_words = num;
        Q_Hash_Table_Test->query_hash_buckets[bucket_num]->match_dist = match_dist;
        Q_Hash_Table_Test->query_hash_buckets[bucket_num]->match_type = match_type;

        Q_Hash_Table_Test->query_hash_buckets[bucket_num]->next = NULL;
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
        Query* temp = Q_Hash_Table_Test->query_hash_buckets[bucket_num];
        Q_Hash_Table_Test->query_hash_buckets[bucket_num] = bucket;
        bucket->next = temp;
    }
    return EC_SUCCESS;
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

//bloom filter used in exact match
void deduplicate_edit_distance(const char* temp, unsigned int , int , int,bk_index* ix);
void deduplicate_exact_matching(const char* temp, unsigned int , int , int);
void deduplicate_hamming(const char* temp, unsigned int , int , int);

void add_payload(struct Payload* payload,int queryId, int dist){
    Payload* new_payload = malloc(sizeof(Payload));
    new_payload->queryId = queryId;
    new_payload->threshold = dist;
    new_payload->next = payload->next;
    payload->next = new_payload;
}
bk_index ix;

ErrorCode StartQuery_QueryTest_Insert(QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist){
    add_query_Test(query_id%QUERY_HASH_BUCKETS_TEST,query_id,query_str, match_type, match_dist);
    
    //update appropriate data structures
    if(match_type == 0){
        deduplicate_exact_matching(query_str, query_id, match_dist, match_type);
    }else if(match_type == 1){
        deduplicate_hamming(query_str, query_id, match_dist, match_type);
    }else if(match_type == 2){
        deduplicate_edit_distance(query_str, query_id, match_dist, match_type, &ix);
    }

    return EC_SUCCESS;

}

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

void deduplicate_edit_distance(const char* temp, unsigned int queryId, int dist, int type, bk_index* ix){
    char* read_word;
    char* temp_temp = (char*)temp; 
    read_word = strtok(temp_temp, " ");
    while(read_word != NULL){
        int len = strlen(read_word);

        int word_hash_value = djb2(read_word)%EDIT_HASH_BUCKETS_TEST;
        
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

unsigned long hash(char *str){
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}
void deduplicate_exact_matching(const char* temp, unsigned int queryId, int dist, int type){
    
    char* read_word;
    char* temp_temp = (char*)temp; 
    read_word = strtok(temp_temp, " ");
    while(read_word != NULL){
        int len = strlen(read_word);
        //printf("%s %d %d %d \n",read_word ,queryId, dist, type );

       // bloom_filter_exact[djb2(read_word)%10] = 1;
       // bloom_filter_exact[jenkins(read_word)%10] = 1;

/*
        int word_hash_value = hash(read_word)%EXACT_HASH_BUCKET_TEST;
        //printf("Word %s hashes to j:%d dj:%d\n",read_word,word_hash_value1,word_hash_value2);
        
        if(hash_table_exact[len-1] == NULL){

            //initialize hashtable 
            hash_table_exact[len-1] = malloc(sizeof(Hash_table_exact));
            hash_table_exact[len-1]->hash_buckets = malloc(sizeof(entry)*EXACT_HASH_BUCKET_TEST);

            //initialize hash buckets
            for(int i=0 ; i<= EXACT_HASH_BUCKET_TEST-1 ;i++){
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
        }*/
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

        int word_hash_value = hash(read_word)%HAMMING_HASH_BUCKET_TEST;
        //printf("Word %s hashes to %d\n",read_word,word_hash_value);
        if(hash_tables_hamming[len-1] == NULL){

            //initialize hashtable 
            hash_tables_hamming[len-1] = malloc(sizeof(Hash_table));
            hash_tables_hamming[len-1]->hash_buckets = malloc(sizeof(Hash_Bucket*)*HAMMING_HASH_BUCKET_TEST);
            
            //initialize hash buckets
            for(int i=0 ; i<= HAMMING_HASH_BUCKET_TEST-1 ;i++){
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
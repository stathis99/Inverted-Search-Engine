//used for testing
#define QUERY_HASH_BUCKETS_TEST 4
void InitializeIndex_QueryTest();
ErrorCode StartQuery_QueryTest(QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist);
ErrorCode add_query_Test(int bucket_num, QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist);
ErrorCode StartQuery_QueryTest(QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist);
int hamming_dist(const char* str1, const char* str2, int len1, int len2);
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
Query_Hash_Table* Q_Hash_Table_Test;
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

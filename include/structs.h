#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>

#define QUERY_HASH_BUCKETS 100
#define BLOOM_FILTER_SIZE 500
#define EDIT_HASH_BUCKETS 100
#define WORD_HASH_TABLE_BUCKETS 100
#define EXACT_HASH_BUCKET 100
#define HAMMING_HASH_BUCKET 100

#define MAX_DOC_LENGTH (1<<22)
#define MAX_WORD_LENGTH 31
#define MIN_WORD_LENGTH 4
#define MAX_QUERY_WORDS 5
#define MAX_QUERY_LENGTH ((MAX_WORD_LENGTH+1)*MAX_QUERY_WORDS)

// Mathching types
typedef enum{ MT_EXACT_MATCH, MT_HAMMING_DIST, MT_EDIT_DIST }MatchType;

/// Error codes:			
typedef enum{ EC_SUCCESS, EC_NO_AVAIL_RES, EC_FAIL }ErrorCode;

enum error_code { SUCCESS = 0, ERROR = 1, NULL_POINTER = 2};
enum match_type { EDIT_DIST = 1, HAMMING_DIST = 2};


//============================================================ giti exoume 2 type defs


//structs for entry. Entry stores a word and has a payload with all the queries that contain this word.
typedef char word;

typedef struct Payload{
    unsigned int queryId;
    int threshold;
    struct Payload* next;
}Payload;

typedef struct Entry{
    Payload* payload;
    word this_word[32];
    struct Entry* next;
}Entry;
typedef Entry *entry;


//struct for bk tree 
typedef struct Index{
    word this_word[32];
    int len;
    int weight;
    struct Index* next;
    struct Index* child;
    Payload* payload;
}Index;
typedef Index *bk_index;


typedef struct result_node{
    struct result_node* next;
    entry this_entry;
}result_node;

typedef struct result_node_bk{
    struct result_node_bk* next;
    bk_index this_entry;
}result_node_bk;


typedef struct Hash_table{
    struct Hash_Bucket** hash_buckets;
    //struct Hash_Bucket* hash_buckets[10];
}Hash_table;

typedef struct Hash_Bucket{
    struct Hash_Bucket* next;
    bk_index node;
}Hash_Bucket;

typedef struct Hash_table_exact{
    entry* hash_buckets;
}Hash_table_exact;

//structures for query data storage
typedef unsigned int QueryID;
typedef struct Query{
	QueryID query_id;
	char str[MAX_QUERY_LENGTH];
	MatchType match_type;
	unsigned int match_dist;
    struct Query* next;
    char words[5][32];
    int query_words;
}Query;

typedef struct Query_Hash_Table{
    Query** query_hash_buckets;
}Query_Hash_Table;

typedef struct query_ids{
    QueryID queryID;
    struct query_ids* next;
}query_ids;

typedef struct Words_Hash_Bucket{
    word this_word[32];
    struct Words_Hash_Bucket* next;
}Words_Hash_Bucket;

typedef struct Words_Hash_Table{
    Words_Hash_Bucket* words_hash_buckets[WORD_HASH_TABLE_BUCKETS];
}Words_Hash_Table;



static int distance (const char * word1,int len1,const char * word2, int len2);
int hamming_distance(const char* str1, const char* str2, int len);
enum error_code create_entry(const word* w, entry* e,unsigned int queryId,int dist);
enum error_code destroy_entry(entry* e);
enum error_code add_entry_no_list(entry first, const entry new_entry);
enum error_code lookup_entry_index(const word* w,int docWordLen, bk_index* ix, int threshold,int match_type);
void print_bk_tree(bk_index ix,int pos);
enum error_code destroy_entry_index(bk_index* ix);
bk_index bk_create_node(bk_index* ix,const word* entry_word,int weight, int queryId, int dist);
int count_documents(FILE* fp);
enum error_code look_for_threshold(struct Payload* payload,int threshold,const word* q_w,const word* w,int match_type ,bk_index temp);
int bk_add_node(bk_index* ix,const word* entry_word,int qWordLen,enum match_type type, bk_index* node, int queryId, int dist);
unsigned long hash(unsigned char *str);
void delete_hash_tables_edit();
void print_hash_tables(Hash_table** hash_table);
void deduplicate_edit_distance(const char* temp, unsigned int , int , int,bk_index* ix);
void deduplicate_edit_distance2(const char* temp, unsigned int , int , int, Hash_table** hash_table,bk_index* ix);
void deduplicate_exact_matching(const char* temp, unsigned int , int , int);
void deduplicate_hamming(const char* temp, unsigned int , int , int);
void print_hash_table_exact(Hash_table_exact** hash_table_exact);
void delete_hash_tables_hamming();
void delete_hash_tables_exact();
ErrorCode DestroyIndex();
typedef unsigned int DocID;
void add_payload(struct Payload* payload,int queryId, int dist);
ErrorCode MatchDocument(DocID doc_id, const char* doc_str);
ErrorCode InitializeIndex();
unsigned int djb2(const void *_str);
unsigned int jenkins(const void *_str);
ErrorCode StartQuery (QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist);
ErrorCode add_query(int bucket_num, QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist);
void print_query_list();
void lookup_exact(const word* w,Hash_table_exact** hash_table_exact);
ErrorCode EndQuery(QueryID query_id);
void print_query_hash_buckets();
void free_words_hash_table();
void reset_words_hash_table();

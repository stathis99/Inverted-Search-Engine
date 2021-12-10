#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdint.h>

#define QUERY_HASH_BUCKETS 100
#define BLOOM_FILTER_SIZE 500
#define EDIT_HASH_BUCKETS 100

typedef char word;

typedef struct Payload{
    unsigned int queryId;
    int threshold;
    struct Payload* next;
}Payload;

//structs for entry list
typedef struct Entry{
    Payload* payload;
    word* this_word;
    struct Entry* next;
}Entry;
typedef Entry *entry;

typedef struct result_node{
    struct result_node* next;
    entry this_entry;
}result_node;


typedef struct Entry_List{
    entry first_node;
    entry last_node;
    int node_count;
}Entry_List;
typedef Entry_List *entry_list;

//struct for bk tree
typedef struct Index{
    word* this_word;
    int weight;
    struct Index* next;
    struct Index* child;
    Payload* payload;
}Index;
typedef Index *bk_index;

typedef struct result_node_bk{
    struct result_node_bk* next;
    bk_index this_entry;
}result_node_bk;

enum error_code { SUCCESS = 0, ERROR = 1, NULL_POINTER = 2};
enum match_type { EDIT_DIST = 1, HAMMING_DIST = 2};


void free_word(word* w);
enum error_code create_entry(const word* w, entry* e,unsigned int queryId,int dist);
enum error_code destroy_entry(entry* e);
enum error_code create_entry_list(entry_list* el);
enum error_code add_entry(entry_list* el, const entry* e);
enum error_code destroy_entry_list(entry_list* el);
unsigned int get_number_entries(const entry_list* el);
void print_list(entry_list el);
entry* get_first(const entry_list* el);
entry* get_next(const entry_list* el, entry* e);

//distance functions
int min2(int x, int y);
int min3(int x, int y, int z);
entry_list read_document(int* number);
int edit_distance(const char* str1, const char* str2, int len1, int len2);
int hamming_distance(const char* str1, const char* str2, int len);
enum error_code build_entry_index_sort(const entry_list* el, enum match_type type, bk_index* ix);
enum error_code lookup_entry_index(const word* w, bk_index* ix, int threshold,int match_type);
void print_bk_tree(bk_index ix,int pos);
enum error_code destroy_entry_index(bk_index* ix);
bk_index bk_create_node(bk_index* ix,word* entry_word,int weight, int queryId, int dist);
entry_list read_queries(int* number,FILE* fp);
int count_documents(FILE* fp);
entry_list* read_documents(int* number,FILE* fp,int number_of_documents);
void check_entry_list(const entry_list doc_list, bk_index* ix,int threshold);
enum error_code look_for_threshold(struct Payload* payload,int threshold,const word* q_w,const word* w,int match_type ,bk_index temp);

//new functions to create BK without sorting inner nodes
int bk_add_node(bk_index* ix,word* entry_word,enum match_type type, bk_index* node, int queryId, int dist);
enum error_code build_entry_index(const entry_list* el, enum match_type type, bk_index* ix);


typedef enum{
    /**
    * Two words match if they are exactly the same.
    */
    MT_EXACT_MATCH,
    /**
    * Two words match if they have the same number of characters, and the
    * number of mismatching characters in the same position is not more than
    * a specific threshold.
    */
    MT_HAMMING_DIST,
    /**
    * Two words match if one of them can can be transformed into the other word
    * by inserting, deleting, and/or replacing a number of characters. The number
    * of such operations must not exceed a specific threshold.
    */
    MT_EDIT_DIST
}MatchType;

/// Error codes:			
typedef enum{
    /**
    * Must be returned by each core function unless specified otherwise.
    */
    EC_SUCCESS,
    /**
    * Must be returned only if there is no available result to be returned
    * by GetNextAvailRes(). That is, all results have already been returned
    * via previous calls to GetNextAvailRes().
    */
    EC_NO_AVAIL_RES,
    /**
    * Used only for debugging purposes, and must not be returned in the
    * final submission.
    */
    EC_FAIL
}ErrorCode;


/// Maximum document length in characters.
#define MAX_DOC_LENGTH (1<<22)

/// Maximum word length in characters.
#define MAX_WORD_LENGTH 31

/// Minimum word length in characters.
#define MIN_WORD_LENGTH 4

/// Maximum number of words in a query.
#define MAX_QUERY_WORDS 5

/// Maximum query length in characters.
#define MAX_QUERY_LENGTH ((MAX_WORD_LENGTH+1)*MAX_QUERY_WORDS)



typedef struct Hash_table{
    struct Hash_Bucket** hash_buckets;
    //struct Hash_Bucket* hash_buckets[10];
}Hash_table;

typedef struct Hash_Bucket{
    struct Hash_Bucket* next;
    bk_index node;
}Hash_Bucket;

unsigned long hash(unsigned char *str);
void delete_hash_tables_edit();
void print_hash_tables(Hash_table** hash_table);

typedef struct Hash_table_exact{
    entry* hash_buckets;
}Hash_table_exact;

void deduplicate_edit_distance(const char* temp, unsigned int , int , int, bk_index* ix);
void deduplicate_edit_distance2(const char* temp, unsigned int , int , int, Hash_table** hash_table,bk_index* ix);
void deduplicate_exact_matching(const char* temp, unsigned int , int , int, Hash_table_exact** hash_table_exact);
void deduplicate_hamming(const char* temp, unsigned int , int , int, Hash_table** hash_table,bk_index* hamming_root_table);
enum error_code add_entry_no_list(entry first, const entry new_entry);
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

ErrorCode StartQuery (QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist);
ErrorCode add_query(int bucket_num, QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist);
void print_query_list();
void lookup_exact(const word* w,Hash_table_exact** hash_table_exact);
void match_query(result_node* head);

typedef struct query_ids{
    QueryID queryID;
    struct query_ids* next;
}query_ids;

ErrorCode EndQuery(QueryID query_id);

void print_query_hash_buckets();
int strlen_my();
uint32_t gatopeich_strlen32(const char* str);
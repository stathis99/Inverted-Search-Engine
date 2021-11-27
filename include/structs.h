#include <stdio.h>

typedef struct word{
    char* key_word;
}word;

//structs for entry list
typedef struct Entry{
    char** payload;
    word* this_word;
    struct Entry* next;
}Entry;
typedef Entry *entry;

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
}Index;
typedef Index *bk_index;


enum error_code { SUCCESS = 0, ERROR = 1, NULL_POINTER = 2};
enum match_type { EDIT_DIST = 1, HUMMING_DIST = 2};


void free_word(word* w);
enum error_code create_entry(const word* w, entry* e);
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
int edit_Dist(char* str1, char* str2, int len1, int len2);
int humming_distance(char* str1, char* str2, int m,int n);
enum error_code build_entry_index_sort(const entry_list* el, enum match_type type, bk_index* ix);
enum error_code lookup_entry_index(const word* w, bk_index* ix, int threshold, entry_list* result);
void print_bk_tree(bk_index ix,int pos);
enum error_code destroy_entry_index(bk_index* ix);
bk_index bk_create_node(bk_index* ix,word* entry_word,int weight);
entry_list read_queries(int* number,FILE* fp);
int count_documents(FILE* fp);
entry_list* read_documents(int* number,FILE* fp,int number_of_documents);
void check_entry_list(const entry_list doc_list, bk_index* ix,int threshold);


//new functions to create BK without sorting inner nodes
int bk_add_node(bk_index* ix,word* entry_word,enum match_type type, bk_index* node);
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
}Hash_table;

typedef struct Hash_Bucket{
    struct Hash_Bucket* next;
    bk_index node;
}Hash_Bucket;


void deduplicate_edit_distance(const char* temp, unsigned int , int , int, Hash_table** hash_table,bk_index* ix);
unsigned long hash(unsigned char *str);
void delete_hash_tables(Hash_table**);
void print_hash_tables(Hash_table** hash_table);

typedef struct Payload{
    unsigned int queryId;
    int threshold;
}Payload;


void deduplicate_exact_matching(const char* temp, unsigned int , int , int, Hash_table** hash_table,bk_index* ix, int* bloom_filter);




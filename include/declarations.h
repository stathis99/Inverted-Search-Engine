#include <stdio.h>

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

enum error_code { SUCCESS = 0, ERROR = 1, NULL_POINTER = 2};
enum match_type { EDIT_DIST = 1, HUMMING_DIST = 2};

typedef char* word;

//structs for entry list
typedef struct Entry{
    //char** payload;
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

//entry functions
enum error_code create_entry(const word* w, entry* e);
enum error_code destroy_entry(entry* e);

//entry list functions
enum error_code create_entry_list(entry_list* el);
enum error_code add_entry(entry_list* el, const entry* e);
enum error_code destroy_entry_list(entry_list* el);
unsigned int get_number_entries(const entry_list* el);
void print_list(entry_list el);
entry* get_first(const entry_list* el);
entry* get_next(const entry_list* el, entry* e);

//distance functions
int edit_dist(char* str1, char* str2, int len1, int len2);
int humming_distance(char* str1, char* str2, int len);
int min_of_2(int x, int y);
int min_of_3(int x, int y, int z);


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

//struct for Query data
typedef unsigned int Query_id;
typedef struct Query{
	Query_id query_id;
	char str[MAX_QUERY_LENGTH];
	MatchType match_type;
	unsigned int match_dist;
    struct Query* next;
}Query;
typedef Query *query;

typedef struct{
    query head;
    int number_of_queries;
}Query_List;
typedef Query_List *query_list;

//query and query_list functions 
enum error_code add_query(query my_query);
enum error_code destroy_query_list();
void print_query_list();

//struct for Document data
typedef unsigned int Doc_ID;

//document functions
ErrorCode StartDocument(Doc_ID doc_id, const char* doc_str);

//interface functions
ErrorCode InitializeIndex();
ErrorCode DestroyIndex();
ErrorCode StartQuery(Query_id query_id, const char* query_str, MatchType match_type, unsigned int match_dist);

typedef struct {
    struct AVL_tree* right;
    struct AVL_tree* left;
    word* key_word;
    int height;
}AVL_tree;
typedef AVL_tree* avl_tree;

//avl tree functions
void deduplicate_edit_distance(avl_tree* avl_trees, const char* temp);

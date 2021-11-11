#include <stdio.h>

typedef struct word{
    char* key_word;
}word;

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


//Entry* create_entry();

//enum error_code create_entry(const word* w, entry** e);
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
int editDist(char* str1, char* str2, int m, int n);
int humming_distance(char* str1, char* str2, int m,int n);


enum error_code build_entry_index(const entry_list* el, enum match_type type, bk_index* ix);
enum error_code lookup_entry_index(const word* w, bk_index* ix, int threshold, entry_list* result);
void print_bk_tree(bk_index ix,int pos,int* number);
enum error_code destroy_entry_index(bk_index* ix);
void bk_create_node(bk_index* ix,word* entry_word,int weight);
int bk_add_node(bk_index* ix,word* entry_word,enum match_type type);
entry_list read_queries(int* number,FILE* fp);
int count_documents(FILE* fp);
entry_list* read_documents(int* number,FILE* fp,int number_of_documents);
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


enum error_code { SUCCESS = 0, ERROR = 1};
enum match_type { EDIT_DIST = 0};


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
//enum error_code build_entry_index(const entry_list* el, enum match_type type, index* ix);
/*int min2(int x, int y);
int min3(int x, int y, int z);
int edit_distance(char* str1, char* str2, int m, int n);
int humming_distance(char* str1, char* str2, int m,int n);*/
char** read_document(int* number);

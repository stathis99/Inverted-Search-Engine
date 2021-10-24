#include <stdio.h>

typedef struct word{
    char* key_word;
}word;

typedef struct{
    char** payload;
    word* this_word;
}Entry;
typedef Entry *entry;

typedef struct Entry_List{
    entry entry_node;
    struct Entry_List *next;
}Entry_List;
typedef Entry_List *entry_list;

enum error_code { SUCCESS = 0, ERROR = 1};
//Entry* create_entry();

//enum error_code create_entry(const word* w, entry** e);
enum error_code create_entry(const word* w, entry* e);
enum error_code destroy_entry(entry* e);
enum error_code create_entry_list(entry_list* el);
enum error_code destroy_entry_list(entry_list* el);
unsigned int get_number_entries(const entry_list* el);
enum error_code add_entry(entry_list* el, const entry* e);
void print_list(entry_list el);
entry* get_first(const entry_list* el);
entry* get_next(const entry_list* el);

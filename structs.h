#include <stdio.h>

typedef struct word{
    char* key_word;
}word;

typedef struct{
    char** payload;
    word* this_word;
}Entry;

typedef Entry *entry;
enum error_code { SUCCESS = 0, ERROR = 1};
//Entry* create_entry();

//enum error_code create_entry(const word* w, entry** e);
enum error_code create_entry(const word* w, entry* e);
enum error_code destroy_entry(entry* e);

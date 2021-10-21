#include <stdio.h>

typedef struct word{
    char* key_word;
}word;

typedef struct entry{
    word* this_word;
    char** payload;
}entry;

enum error_code { SUCCESS = 0, ERROR = 1};
//Entry* create_entry();

enum error_code create_entry(const word* w, entry* e);
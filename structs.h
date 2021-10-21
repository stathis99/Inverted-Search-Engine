#include <stdio.h>

typedef struct entry{
    char* word;
    char** payload;
}entry;

typedef struct word{
    char* keyword;
}word;

enum error_code { SUCCESS = 0, ERROR = 1};
//Entry* create_entry();

enum error_code create_entry(const word* w, entry* e);
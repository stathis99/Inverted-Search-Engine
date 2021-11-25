#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "declarations.h"

//array used to pass all query words
char temp[MAX_DOC_LENGTH];

int main(void){

    //initializations here
    InitializeIndex();

    //open our input file
    FILE* fp=fopen("input.txt", "rt");
    if(fp == NULL){
        printf("Cannot Open File input.txt\n\n");
        return -1;
    }
    int num_cur_results = 0;

    //read and store all queries
    char ch;
	unsigned int query_id;
    int fres;

    //used to deduplicate edit distance queries
    //each length has its own avl tree
    avl_tree* avl_trees = malloc(sizeof(avl_tree)* 29);
    for(int i = 0; i <=28 ; i++){
        avl_trees[i] = NULL;
    }

    //start processing queries
    while(1){
        fres = fscanf(fp, "%c %u ", &ch, &query_id);
        if(fres == EOF){
            break;
        }
		if(ch == 's'){
            printf("Reading query %d\n",query_id);
            int match_type;
			int match_dist;

			if(EOF == fscanf(fp, "%d %d %*d %[^\n\r] ", &match_type, &match_dist, temp)){
				printf("Corrupted Test File at Read Queries.\n");
				exit(-1);
			}

            //process files with match_type == 2
            if(match_type == 2){
                //ErrorCode err = StartQuery(query_id, temp, (MatchType)match_type, match_dist);
                //deduplicate query words, insert them into the list
                deduplicate_edit_distance(avl_trees, temp);
                exit(-1);
            }
        }else{
            break;
        }
		
    }

    print_query_list();

    //read all documents
    unsigned int doc_id;
    unsigned int number_of_words;
    while(1){
        if(fres == EOF){
            break;
        }
        if(EOF==fscanf(fp, "%*u %[^\n\r] ", temp)){
            printf("Corrupted Test File at Read Documents.\n");
			exit(-1);
        }
        //ErrorCode err = MatchDocument(doc_id,temp);
        printf("%s",temp);
        fres = fscanf(fp, "%c %u ", &ch, &doc_id);
    }

    //do necessary destructions
    DestroyIndex();
    fclose(fp);
    for(int i = 0; i <=28 ; i++){
        free(avl_trees[i]);
    }
    free(avl_trees);
    printf("\n\nProgram exiting\n");
    return 0;
}

/*
    //create word 1
    char key_word1[] = "hell";
    word my_word1 = (word)malloc(sizeof(key_word1));
    strcpy(my_word1,key_word1);

    char key_word2[] = "hellk";
    word my_word2 = (word)malloc(sizeof(key_word2));
    strcpy(my_word2,key_word2);

    //create entry from the word
    entry my_entry = NULL;
    create_entry(&my_word1,&my_entry);

    //create and initialize an entry list
    entry_list my_entry_list = NULL;
    create_entry_list(&my_entry_list); 

    //add it to the list
    add_entry(&my_entry_list,&my_entry);

    create_entry(&my_word2,&my_entry);

    add_entry(&my_entry_list,&my_entry);

    entry* first = get_first(&my_entry_list);
    printf("first:%s\n",(*first)->this_word->key_word);
    entry* next = get_next(&my_entry_list,first);
    printf("first:%s\n",(*next)->this_word->key_word);
    print_list(my_entry_list);
    destroy_entry_list(&my_entry_list);*/


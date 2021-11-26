#include  "../include/acutest.h"			
#include "../include/structs.h"

void test_entry_list_create(){
	//initialize entry list
	entry_list my_entry_list ;
	create_entry_list(&my_entry_list);
	TEST_CHECK(my_entry_list != NULL);
		
	//destroy list
	destroy_entry_list(&my_entry_list);

}

void test_entry_list_destroy(){
	//initialize entry list
	entry_list my_entry_list ;
	create_entry_list(&my_entry_list);
		
	//destroy list
	destroy_entry_list(&my_entry_list);
	TEST_CHECK(get_number_entries(&my_entry_list) == 0);

}

void test_entry_list_add(void) {
	//initialize entry list
	entry_list my_entry_list ;
	create_entry_list(&my_entry_list);

	//create an entry 
	char key_word[] = "small";
   	word* my_word = (word*)malloc(sizeof(word));
   	my_word->key_word = (char*)malloc(sizeof(char)*sizeof(key_word));
   	strcpy(my_word->key_word,key_word);
	entry my_entry = NULL;
   	create_entry(my_word,&my_entry);
	free_word(my_word);

	//insert it to list
   	add_entry(&my_entry_list,&my_entry);

	TEST_CHECK(get_number_entries(&my_entry_list) == 1);

	destroy_entry_list(&my_entry_list);
}

void test_entry_list_add_second(void) {
	//initialize entry list
	entry_list my_entry_list ;
	create_entry_list(&my_entry_list);

	//create an entry 
	char key_word[] = "small";
   	word* my_word = (word*)malloc(sizeof(word));
   	my_word->key_word = (char*)malloc(sizeof(char)*sizeof(key_word));
   	strcpy(my_word->key_word,key_word);
	entry my_entry = NULL;
   	create_entry(my_word,&my_entry);
	free_word(my_word);

	//insert it to list
   	add_entry(&my_entry_list,&my_entry);
	
	//create a new entry 
	char key_word_new[] = "smell";
   	word* my_word_new = (word*)malloc(sizeof(word));
   	my_word_new->key_word = (char*)malloc(sizeof(char)*sizeof(key_word_new));
   	strcpy(my_word_new->key_word,key_word_new);
	entry my_entry_new = NULL;
   	create_entry(my_word_new,&my_entry_new);
	free_word(my_word_new);
	
	//add the new entry to the list
	add_entry(&my_entry_list,&my_entry_new);

	//check that the first is indeed there
	entry* temp = get_first(&my_entry_list);
	TEST_CHECK(my_entry == *temp);

	//destroy list
	destroy_entry_list(&my_entry_list);
}


void test_bk_tree_empty(void){
	entry_list my_entry_list ;
	create_entry_list(&my_entry_list);
	bk_index ix = NULL;
	//enum error_code result = build_entry_index(&my_entry_list,EDIT_DIST,&ix);
	TEST_CHECK(ix == NULL);
	destroy_entry_list(&my_entry_list);
}

void test_bk_tree(void){
	entry_list my_entry_list ;
	create_entry_list(&my_entry_list);

	//create an entry 
	char key_word[] = "small";
   	word* my_word = (word*)malloc(sizeof(word));
   	my_word->key_word = (char*)malloc(sizeof(char)*sizeof(key_word));
   	strcpy(my_word->key_word,key_word);
	entry my_entry = NULL;
   	create_entry(my_word,&my_entry);
	free_word(my_word);

	//insert it to list
   	add_entry(&my_entry_list,&my_entry);
	
	bk_index ix = NULL;
	//enum error_code result = build_entry_index(&my_entry_list,EDIT_DIST,&ix);
	TEST_CHECK(ix != NULL);

	destroy_entry_index(&ix);
	destroy_entry_list(&my_entry_list);
}

TEST_LIST = {
	{ "Initialize Entry List", test_entry_list_create },
	{ "Free'ed Entry List Number of Entries", test_entry_list_destroy },
	{ "First Entry of List", test_entry_list_add },
	{ "First Entry of List", test_entry_list_add_second },
	{ "Test BK with Empty List", test_bk_tree_empty },
	{ "Test BK with List", test_bk_tree },
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};

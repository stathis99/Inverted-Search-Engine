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

	//insert it to list
   	add_entry(&my_entry_list,&my_entry);

	TEST_CHECK(get_number_entries(&my_entry_list) == 1);
	
	//destroy list
	destroy_entry_list(&my_entry_list);
}

void test_bk_tree_empty(void){
	entry_list my_list ;
	create_entry_list(&my_list);
	bk_index ix;
	enum error_code result = build_entry_index(&my_list,EDIT_DIST,&ix);
	if(result != NULL_POINTER){
		TEST_CHECK(ix != NULL);
		destroy_entry_index(&ix);
	}else{
		TEST_CHECK(1 == 1);
	}
}

void test_bk_tree(void){
	entry_list my_list ;
	create_entry_list(&my_list);
	bk_index ix;
	enum error_code result = build_entry_index(&my_list,EDIT_DIST,&ix);
	if(result != NULL_POINTER){
		TEST_CHECK(ix != NULL);
		destroy_entry_index(&ix);
	}else{
		TEST_CHECK(1 == 1);
	}
}

TEST_LIST = {
	{ "Initialize Entry List", test_entry_list_create },
	{ "Add Entry to Entry List", test_entry_list_add },
	{ "Test BK with Empty List", test_bk_tree_empty },
	{ "Test BK with Empty List", test_bk_tree },
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};
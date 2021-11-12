#include  "../include/acutest.h"			
#include "../include/structs.h"

void test_find_min(void) {

	TEST_CHECK(-1 == -1);

}

TEST_LIST = {
	{ "find_min", test_find_min },
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};
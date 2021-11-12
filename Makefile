#
# In order to execute this "Makefile" just type "make"
#
INCLUDE = ./tests

HEADERS = structs.h
SOURCES0 = main.c functions.c
SOURCES1 = $(INCLUDE)/unit_test.c functions.c
OBJECTS0 = main.o functions.o
OBJECTS1 = $(INCLUDE)/unit_test.o functions.o
OUT		= project1
OUTTEST = test
CC	= gcc

all: $(OBJECTS0) $(OBJECTS1)
	$(CC) -g $(OBJECTS0) -o $(OUT)
	$(CC) -g $(OBJECTS1) -o $(OUTTEST)
	make clean
	
project1: main.o
	$(CC) main.o -o $(OUT)

main.o: main.c
	$(CC) -c main.c 

functions.o:
	$(CC) -c functions.c

unit_test.o: unit_test.c 
	$(CC) -c unit_test.c

clean: 
	rm functions.o main.o $(INCLUDE)/unit_test.o

norun: $(OBJECTS0)
	$(CC) -g $(OBJECTS0) -o $(OUT)

valgrind: $(OBJECTS0) $(OBJECTS1)
	$(CC) -g $(OBJECTS0) -o $(OUT)
	$(CC) -g $(OBJECTS1) -o $(OUTTEST)
	make clean
	valgrind ./$(OUT)



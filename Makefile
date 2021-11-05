HEADERS = structs.h
SOURCES = test.c functions.c
OBJECTS = test.o functions.o
OUT		= output

all: $(OBJECTS)
	gcc -g $(OBJECTS) -o $(OUT)
	make clean
	./output

output: test.o
	gcc test.o -o $(OUT)

test.o: test.c
	gcc -c test.c 

functions.o:
	gcc -c functions.c

clean: 
	rm functions.o test.o output

norun: $(OBJECTS)
	gcc -g $(OBJECTS) -o $(OUT)


HEADERS = structs.h
SOURCES = test.c functions.c
SOURCE1	= unit_test.c
OBJECTS = test.o functions.o
OBJS1   = unit_test.o
OUT		= output
OUTTEST = testing

all: $(OBJECTS) $(OBJS1)
	gcc -g $(OBJECTS) -o $(OUT)
	gcc -g $(OBJS1) -o $(OUTTEST)
	make clean
	./output

output: test.o
	gcc test.o -o $(OUT)

test.o: test.c
	gcc -c test.c 

functions.o:
	gcc -c functions.c

unit_test.o: unit_test.c 
	gcc -c unit_test.c

clean: 
	rm functions.o test.o output

norun: $(OBJECTS)
	gcc -g $(OBJECTS) -o $(OUT)


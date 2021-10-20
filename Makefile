output: test.o
	g++ test.o -o output

test.o: test.c
	g++ -c test.c

clean: 
	rm *.o output
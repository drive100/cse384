
test: test.c
	gcc -o test test.c

clean:
	rm test

tar: 
	tar -cf test.tar test.c makefile
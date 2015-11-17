dns:	main.c constants.h
	gcc -Wall -g -o dns main.c

test:	t.c
	gcc -Wall -g -o test t.c
	
clean:
	rm dns
operacoes: livros.o main.o
	gcc livros.o main.o -o tp1 -Wall -Wextra -Werror -std=c99 -pedantic
main.o: main.c livros.h
	gcc -g -c main.c -Wall -Wextra -Werror -std=c99 -pedantic
livros.o: livros.h livros.c
	    gcc -g -c livros.c -Wall -Wextra -Werror -std=c99 -pedantic
clean:
	rm *.o *.gch
	rm livros

all: first

first: first.c
	gcc -g -Wall -Werror -fsanitize=address first.c -o first

clean:
	rm -f first


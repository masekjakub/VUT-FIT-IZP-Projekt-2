all: 
	gcc -std=c99 -Wall -Wextra -Werror -g setcal.c -o setcal.exe

clean:
	rm -rf *.o
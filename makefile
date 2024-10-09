all: soda-tetris

soda-tetris: winutils.o grid_utils.o figure_move.o main.o 
	gcc -o  soda-tetris  -Wall main.o grid_utils.o figure_move.o winutils.o -lncursesw

main.o: src/main.c
	gcc -c -Wall -o main.o -Wall src/main.c 

figure_move.o: src/figure_move.c
	gcc -c  -Wall -o figure_move.o src/figure_move.c

grid_utils.o: src/grid_utils.c
	gcc -c  -Wall -o grid_utils.o -Wall src/grid_utils.c 

winutils.o: src/winutils.c
	gcc -c  -Wall -o winutils.o src/winutils.c
clean:
	rm -rf *.o soda-tetris

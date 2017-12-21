SRC = main.c variables.c
CFLAGS = -Wall -Wextra
BIN = bin

all:
	mpicc $(SRC) -o $(BIN) $(CFLAGS)

run: all
	mpirun -np 8 --hostfile hostfile $(BIN)

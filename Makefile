SRC = main.cc
CFLAGS = -Wall -Wextra
BIN = bin

all:
	mpicc $(SRC) -o $(BIN) $(CFLAGS)

run: all
	mpirun -np 8 --hostfile hostfile $(BIN)

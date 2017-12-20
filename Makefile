SRC = main.cc
BIN = bin

all:
	mpicc $(SRC) -o $(BIN)

run: all
	mpirun -np 8 --hostfile hostfile $(BIN)

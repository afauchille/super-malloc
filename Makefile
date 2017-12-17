SRC = main.cc
BIN = bin

all:
	mpicc $(SRC) -o $(BIN)

run: all
	mpirun $(BIN) -np 2 -hostfile hostfile

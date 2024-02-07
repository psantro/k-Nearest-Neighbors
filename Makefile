# Ponme un 10 Pepe

# Compiler related
CC := mpicc
CFLAGS = -std=c17 -I $(INC) -fopenmp

# Directories related
INC := inc
SRC := src
BIN := bin
DIRS := $(INC) $(SRC) $(BIN)

# Files related
SRCS := $(wildcard $(SRC)/*.c)
EXE := $(BIN)/kNN.out

all: clean test-build

test-build: $(SRCS) | $(DIRS)
	$(CC) $(CFLAGS) -g $^ -o $(EXE)

release-build: $(SRCS) | $(DIRS)
	$(CC) $(CFLAGS) -O3 -DNDEBUG $^ -o $(EXE)

clean:
	$(RM) $(EXE)

$(INC):
	mkdir $@

$(SRC):
	mkdir $@

$(BIN):
	mkdir $@

.PHONY: all test-build release-build clean

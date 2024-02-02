CC := gcc
CFLAGS := -std=c17 -g -I inc

SRC = src
SRCS := $(wildcard $(SRC)/*.c)

OBJ = obj
OBJS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

BIN = bin
OUT := $(BIN)/kNN.out

.PHONY: all clean

all: clean $(OUT)

release-build:

test-build:

clean:
	$(RM) $(OBJS) $(OUT)

$(OUT): $(OBJS)
	$(CC) $(CFLAGS) $< -o $@

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):
	mkdir $@

$(BIN):
	mkdir $@

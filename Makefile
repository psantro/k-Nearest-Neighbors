CC:=gcc
CFLAGS:=-std=c17 -g

SRC:=src
SRCS:=$(wildcard $(SRC)/*.c)

OBJ:=obj
OBJS:=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

BIN:=bin
OUT:=$(BIN)/kNN.out

.PHONY: all clean

all: clean $(BIN)

clean:
	$(RM) $(OBJS) $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $<

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -o $@ $<

$(SRC):
	makedir $(SRC)

$(OBJ):
	makedir $(OBJ)

$(BIN):
	makedir $(BIN)

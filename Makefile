CC := gcc
CFLAGS := -std=c17 -g

SRC := src
SRCS := $(wildcard $(SRC)/*.c)

OBJ := obj
OBJS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

BIN := bin
OUT := $(BIN)/kNN.out

.PHONY: all clean

all: clean $(OUT)

clean:
	$(RM) $(OBJS) $(OUT)

$(OUT): $(OBJS) $(BIN)
	$(CC) $(CFLAGS) $< -o $@

$(OBJ)/%.o: $(SRC)/%.c $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):
	mkdir $@

$(BIN):
	mkdir $@

ROOTPATH=$(shell pwd)
BIN=compiler
SRC=$(wildcard *.c)
OBJ=$(patsubst %.c,%.o,$(SRC))
WARNINGS=-Wall

$(BIN):$(OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS) -O3 

%.o:%.c
	$(CXX) -c $< $(LDFLAGS) 

.PHONY:clean
clean:
	@rm -rf $(OBJ) $(BIN) 

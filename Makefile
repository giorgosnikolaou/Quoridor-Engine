# Compiler
CC = gcc

# Paths
INCLUDE = ./include
SRC = ./src

# Defines 
DEFINES = 

# Compile options
CFLAGS = -Wall -g3 $(DEFINES) -I$(INCLUDE)

# .o files needed
SRCS    := $(wildcard $(SRC)/*.c)
OBJS    := $(patsubst $(SRC)/%.c,$(SRC)/%.o,$(SRCS))

# Executable
EXEC = main

# Command line arguments
ARGS = 

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

clean:
	@rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)

time: $(EXEC)
	time ./$(EXEC) $(ARGS)
	
valgrind: $(EXEC)
	valgrind --leak-check=full --track-origins=yes -s ./$(EXEC) $(ARGS)
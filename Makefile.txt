# Compiler and compiler flags
CC = g++
CFLAGS = -std=c++17 -pedantic

# Executables
EXECUTABLES = main lsh cube

# Object files
OBJECTS = main.o lsh.o cube.o mnist.o main_helper.o global_functions.o KMeansPLusPlus.o Hypercube.o lsh_class.o

# Targets
all: $(EXECUTABLES)

main: main.o mnist.o main_helper.o global_functions.o KMeansPLusPlus.o Hypercube.o lsh_class.o
	$(CC) $(CFLAGS) -o main main.o mnist.o main_helper.o global_functions.o KMeansPLusPlus.o Hypercube.o lsh_class.o

lsh: lsh.o mnist.o main_helper.o global_functions.o KMeansPLusPlus.o Hypercube.o lsh_class.o
	$(CC) $(CFLAGS) -o lsh lsh.o mnist.o main_helper.o global_functions.o KMeansPLusPlus.o Hypercube.o lsh_class.o

cube: cube.o mnist.o main_helper.o global_functions.o KMeansPLusPlus.o Hypercube.o lsh_class.o
	$(CC) $(CFLAGS) -o cube cube.o mnist.o main_helper.o global_functions.o KMeansPLusPlus.o Hypercube.o lsh_class.o

# Object file dependencies
main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

lsh.o: lsh.cpp
	$(CC) $(CFLAGS) -c lsh.cpp

cube.o: cube.cpp
	$(CC) $(CFLAGS) -c cube.cpp

mnist.o: mnist.cpp
	$(CC) $(CFLAGS) -c mnist.cpp

main_helper.o: main_helper.cpp
	$(CC) $(CFLAGS) -c main_helper.cpp

global_functions.o: global_functions.cpp
	$(CC) $(CFLAGS) -c global_functions.cpp

KMeansPLusPlus.o: KMeansPLusPlus.cpp
	$(CC) $(CFLAGS) -c KMeansPLusPlus.cpp

Hypercube.o: Hypercube.cpp
	$(CC) $(CFLAGS) -c Hypercube.cpp

lsh_class.o: lsh_class.cpp
	$(CC) $(CFLAGS) -c lsh_class.cpp

# Clean rule
clean:
	rm -f $(EXECUTABLES) $(OBJECTS)

.PHONY: all clean

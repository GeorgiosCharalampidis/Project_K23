CC = g++
CFLAGS = -std=c++17

CLUSTER_TARGET = cluster
CLUSTER_SRCS = cluster.cpp mnist.cpp global_functions.cpp KMeansPLusPlus.cpp Hypercube.cpp lsh_class.cpp
CLUSTER_OBJS = $(CLUSTER_SRCS:.cpp=.o)

LSH_TARGET = lsh
LSH_SRCS = lsh.cpp lsh_class.cpp mnist.cpp global_functions.cpp
LSH_OBJS = $(LSH_SRCS:.cpp=.o)

CUBE_TARGET = cube
CUBE_SRCS = cube.cpp Hypercube.cpp mnist.cpp global_functions.cpp
CUBE_OBJS = $(CUBE_SRCS:.cpp=.o)

all: $(CLUSTER_TARGET) $(LSH_TARGET) $(CUBE_TARGET)

$(CLUSTER_TARGET): $(CLUSTER_OBJS)
	$(CC) $(CFLAGS) -o $(CLUSTER_TARGET) $(CLUSTER_OBJS)

$(LSH_TARGET): $(LSH_OBJS)
	$(CC) $(CFLAGS) -o $(LSH_TARGET) $(LSH_OBJS)

$(CUBE_TARGET): $(CUBE_OBJS)
	$(CC) $(CFLAGS) -o $(CUBE_TARGET) $(CUBE_OBJS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(CLUSTER_OBJS) $(LSH_OBJS) $(CUBE_OBJS) $(CLUSTER_TARGET) $(LSH_TARGET) $(CUBE_TARGET)

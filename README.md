# Software Development for Algorithmic Problems

## Assignment 1 - Vector Search and Clustering in C/C++

### Compilation Instructions

The compilation is performed using the `make` command, ensuring you are in the project's directory. The `make clean` command can be used to delete all the object and executable files that were created with `make`.

The compiler used is `g++`, and the C++ version is C++17. Using `make`, all source files are turned into object files, and three different executables (`lsh`, `cube`, and `cluster`) are generated. Each executable has its own target and requires corresponding source files.

### Program Usage Instructions

#### For the `lsh` program:
- Running `./lsh` without input parameters prompts the user to enter the dataset path. After the search structure is built, the program asks for the paths of the query file and the output file. If the parameters k, L, N, R are not provided, it runs with default values.
- Usage example: `./lsh -d input.dat -q query.dat -k 4 -L 5 -o output.dat -N 1 -R 10000`

#### For the `cube` program:
- The logic is similar to the `lsh` program.
- Usage example: `./cube -d input.dat -q query.dat -k 7 -M 10 -probes 2 -o output.dat -N 1 -R 10000`

#### For the `cluster` program:
- Examples of using the command line for different algorithms and settings: 
  - `./cluster -i input.dat -c cluster.conf -o output.txt -m Classic`
  - `./cluster -i input.dat -c cluster.conf -o output.txt -m LSH`
  - `./cluster -i input.dat -c cluster.conf -o output.txt -m HyperCube`
  - Add `-complete` to the command line for generating a different pattern of output.

### Program Description

Users can execute the following programs: `./lsh`, `./cube`, `./cluster`. These programs offer functionalities such as:
- Reading parameters from the command line, including paths for input and output files and other necessary variables.
- Executing functions responsible for reading and correctly storing the MNIST file data in a vector<vector> structure.
- The core code is located in `lsh_class.cpp-h`, `Hypercube.cpp-h`, and `KMeansPlusPlus.cpp-h` for the `lsh`, `cube`, and `cluster` programs, respectively, containing the classes and corresponding functions that are invoked in these programs.

### Key Features of the Programs:

- Capability to read parameters from the command line, such as paths for input/output files and other essential variables.
- Execution of the function `read_mnist_images` responsible for the correct reading and storage of the MNIST file data.
- Detailed description and functionality of the core code, including classes, constructors, and method implementations for LSH, Hypercube, and clustering algorithms.

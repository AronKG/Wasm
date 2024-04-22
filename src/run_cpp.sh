#!/bin/bash

# Compile the C++ code
g++ -o tree tree.cpp

# Run the compiled program
./tree

#run valgrind 
valgrind --leak-check=full ./tree


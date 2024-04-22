#!/bin/bash

echo COMPILE

# Compile the C++ code
rm tree
g++ -o tree tree.cpp

echo RUN

# Run the compiled program
./tree

echo VALGRIND

#run valgrind 
valgrind --leak-check=full ./tree


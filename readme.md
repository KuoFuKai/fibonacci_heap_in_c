# Fibonacci Heap

This project is an implementation of a Fibonacci Heap in C. A Fibonacci Heap is a data structure for priority queue operations, consisting of a collection of heap-ordered trees. It has a better amortized running time than many other priority queue data structures including the binary heap and binomial heap.

## Features

- Insertion of a new element into the heap.
- Finding the minimum element.
- Union of two heaps.
- Extracting the minimum element.
- Decreasing the key of an element.
- Deletion of an element.

## Compilation

Use the following command to compile the program:

```sh
gcc -O3 -std=c11 -Wall -o fib_heap main.c

Usage
To execute the program, you can either directly input the commands via the terminal, or you can redirect the input from a file:

Terminal Input
```sh
./fib_heap

Type your commands followed by the quit command to terminate the program.

File Redirection
```sh
./fib_heap < commands.txt

Create a file with all the commands, then run the program with input redirected from this file:

Commands
The available commands are:

insert <key> <value>: Insert a new element with the given key and value.
delete <key> <value>: Delete the element with the specified key and value.
decrease <key> <value> <amount>: Decrease the key of the specified element by the given amount.
extract: Extract the minimum element from the heap.
quit: Terminate the program.
License
This project is open-sourced under the MIT license.
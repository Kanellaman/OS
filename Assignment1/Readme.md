# Parallel Text Processing Project

## Project Overview

This project consists of code for a specific assignment in a course on Operating Systems. The code is designed to perform various tasks involving processes, shared memory, and synchronization. Below is a brief overview of what the code does:

## Code Files

- **`functions.h`**: Defines data structures and functions used in the project. Notably, it includes structures for semaphores and memory management.

- **`Timer.h`**: Contains timing functionality used in the project, sourced from a parallel systems course.

- **`functions.c`**: Implements key functions, such as initializing memory and semaphore operations.

- **`main.c`**: Represents the main program and parent process. It manages the division of text into segments and coordinates child processes.

- **`execchild.c`**: Represents child processes that perform tasks like requesting segments of text and interacting with shared memory.

## Project Execution

The code functions as follows:

1. The main program (`main.c`) reads a text file and divides it into segments. It then creates multiple child processes using fork.

2. Child processes (`execchild.c`) request segments of text randomly, simulating concurrent operations.

3. The code uses semaphores for synchronization to ensure that child processes access segments safely and efficiently.

4. Shared memory is employed to store and exchange data between processes.

5. The parent process orchestrates the overall execution, coordinating child processes' requests and copying text segments into shared memory.

## Usage

To use this code, follow the instructions provided in the course materials or the assignment guidelines. Ensure that you have the necessary input files and compile the code before running it.
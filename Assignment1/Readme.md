# Project Readme

**Project Overview**

This project aims to analyze the functionality of the code for the first assignment of the course. The code analysis is done file by file, with the exception of `main.c` and `execchild.c`, for which commentary is provided at specific points. It is essential to note that line numbering starts from 0. Thus, when referring to line 1 of a file, it corresponds to line 0 in the program. The maximum number of characters per line is set at 1000 since the input file does not have lines with more characters. Additionally, 640,000 positions are reserved for storing a segment in shared memory.

## `functions.h`

- **Struct semaphore**: This structure stores two semaphores and an integer number. `num` is a request counter initialized to 0. `sp` is a typical semaphore that blocks requests, initialized to 0. `mutex` will be used as a binary semaphore (whose function is equivalent to that of mutex) and is initialized to 1. The `mutex` variable primarily protects changes to the `num` counter.

- **Struct memory**: This structure contains variables that hold information about the text and its segments, along with some semaphores for communication between child processes and the parent. `segm` stores the segment requested by the child, `total` stores the number of completed requests, `requests` stores the number of requests each child must make, and `last_line` is either 0 if the lines in the file are evenly divisible by the number of lines per segment given as an argument, or it holds the remainder of this division, indicating the number of lines in the last segment. Lastly, `line_segm` stores the number of lines per segment, and `total_segs` stores the number of segments into which the text is divided.

- Finally, two variables `smphr` and `mem` are defined, corresponding to pointers to the above two structures.

## `Timer.h`

This file originates from the "Parallel Systems" course and serves as auxiliary code for the first assignment. It is used for timing requests, as required in the assignment.

## `functions.c`

- **get_key()**: Returns the key of the shared memory used for communication among all processes.

- **read_file()**: Returns a line from the file pointed to by the `fp` pointer.

- **init()**: Initializes the elements of a `struct memory` object.

- The next two functions, `post` and `wait`, respectively, perform semaphore operations on the object.

## `main.c`

This file represents the implementation of the parent process. Notably, the contents of the text file passed as an argument are stored in the `array` variable, which is a two-dimensional character array. Each line `i` of the array is stored in the corresponding `array[i]` position. Using this array, each segment requested by child processes is copied to the shared memory.

## `execchild.c`

To ensure the randomness of values per child process, the `seed` variable is initialized with the time at which the code is run plus the process's ID. Later, `seed` is passed as an argument to the `rand_r()` function. Shared memory is connected to the child process, and if no errors occur, execution continues.

## `main.c` and `execchild.c`

After the parent process connects to shared memory and performs necessary initializations, it creates `N` child processes using the `fork()` function. These child processes are responsible for executing the code in `execchild.c`, with each child receiving its unique ID as an argument.

Each child process, after performing necessary preparations, starts making requests within a loop. When a child process randomly selects a line from the segment it desires using the `rand_r()` function, it increments or decrements the counter `sp[x].num`, where `x` is the segment number in question. This counter is protected by `sp[x].mutex` to prevent race conditions.

Each child process that requests a segment behaves differently based on the value of `sp[x].num`:
1. If `sp[x].num` equals 1, it means the child process is the first to request segment `x`, so it waits at semaphore `k->next`.
2. If `sp[x].num` is not equal to 1, it means another process has already requested segment `x`, and they will be serviced concurrently without mutual exclusion once the first process passes. Therefore, it is blocked by semaphore `sp[x].sp`.

This design ensures that each child process waiting for a particular segment waits at `k->next` if it's the first requester, or `sp[x].sp` if other processes have requested the same segment before. Meanwhile, the parent process, upon entering the `while` loop, clears the `str` variable where the segment will be stored and raises the semaphore for `k->next` to allow the first child process to request its segment. Afterward, it waits at semaphore `k->sp2`. Once the child process updates the `k->segm` field with the segment it wants, it raises the `k->sp2` semaphore to allow the parent process to copy the required segment to

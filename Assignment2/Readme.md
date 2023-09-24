# Lazy Allocation in xv6

## Project Overview

This project introduces lazy allocation support to the xv6 operating system. Lazy allocation is a memory management technique that defers the actual allocation of physical memory until it is accessed by a process. This can help improve memory utilization and system performance.

### Background

xv6 is a simple and educational Unix-like operating system used in various educational settings. It is known for its simplicity and ease of understanding. However, it lacks support for lazy allocation by default.

### Project Objective

The main objective of this project is to implement lazy allocation in the xv6 operating system. Lazy allocation involves making changes to the page fault handler and memory management subsystem to allocate physical memory only when a page is accessed for the first time.

## Code Modifications

- **Page Fault Handler**: The page fault handler in xv6 is extended to handle page allocation on-demand. When a page fault occurs, the handler allocates a physical page and map it to the corresponding virtual address only when the page is accessed for the first time.

- **Memory Management**: Changes were made in the memory management subsystem to keep track of allocated and unallocated pages efficiently. The system should maintain information about which pages are currently in use and which pages are available for allocation.

- **User-Level Interface**: The project involves adding user-level system calls and library functions to allow user processes to request lazy allocation explicitly.

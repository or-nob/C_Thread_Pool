# C Thread Pool
## A simple thread pool in ANSI-C.

This provides a simple list of apis to create a thread pool of a certain size. It then recycle through limited resource to complete jobs submitted in the pool queue. It works using POSIX threads in Unix/Unix-like systems.

## Example
Please look at the ```demo.c``` file for better understanding of various apis.

## Features

- Has an underlying list of POSIX threads who complete queued jobs in an multi-threaded environement.

## Installation

This requires [Clang](https://clang.llvm.org/)/[GCC](https://gcc.gnu.org/) to run.

To compile and test the demo.c file.

```sh
make
./bin/pool
```

## License

MIT

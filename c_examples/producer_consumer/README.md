# Producer-Consumer Simulation

This project simulates a producer-consumer problem using pthreads in C. 

## Running

Compile and run Unit Tests:

```
make tests
```

Compile and run the project with verbosity turned on and in Debug mode (NDEBUG not defined; assert statements included):

```
make run
```

Compile and run the project with verbosity turned off in Release mode (NDEBUG is defined and assert statements will be removed):

```
make run BUILD_TYPE=Release VERBOSE=0
```

Run concurrency checks via helgrind:

```
make check_concurrency
```

Run memory checks via valgrind:

```
make check_memory
```

Or run both concurrency and memory checks with:

```
make valhella
```

You can change various options in the Makefile (e.g. `NUM_PRODUCERS`, `NUM_CONSUMERS`, `BUFFER_SIZE`, `VERBOSE`, `BUILD_TYPE`, etc.)

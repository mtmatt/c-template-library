# C Data Structures Library

## Installation

```bash
git clone https://github.com/mtmattqq/c-data-structures.git # or download the zip
cd c-data-structures
make
```

## Usage

### Include the header file

```c
#include <cds/array.h>
#include <cds/stack.h>
#include <cds/queue.h>
#include <cds/util.h>
// ...
```

### Compile

```bash
gcc -o main main.c -I/path/to/c-data-structures/include -L/path/to/c-data-structures/lib -lcds
```

### Functions in common

1. `cds_<data structure name>_new`: create a new data structure
2. `cds_<data structure name>_delete`: free the memory used by a data structure
3. `cds_<data structure name>_size`: get the number of elements in a data structure
4. `cds_<data structure name>_empty`: check if a data structure is empty

### Data structures

1. Array
2. Stack
3. Queue
4. List (Doubly Link List)

### Utilities

1. `ERR_EXIT(s)`: print an error message and exit
2. `CONV(type)`: convert a pointer to a type and dereference it

```c
CONV(int32_t) cds_stack_top(&stack);
// is equivalent to
*(int32_t*) cds_stack_top(&stack);
```

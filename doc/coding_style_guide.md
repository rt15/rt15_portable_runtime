# File names

* No upper case characters.
* Words are separated by underscores.

# Commit messages

* Starts with an upper case character.
* Use imperative: "Fix bug", "Close file"...
* 72 characters max for the first line.
* The second line must be blank.

# Header guards

If the header file name is rt_types.h then the header guard must look like:
```c
#ifndef RT_TYPES_H
#define RT_TYPES_H

#endif /* RT_TYPES_H */
```

All source files must end with an empty line.

# Macros

* Macros are never indented.

# Parameters and local variables names

* Simple snake names (words_count). 

# function names

* Snake names starting with the module name (rt_memory_move).

# Local variables declarations

* No need to align variables names.
* All variables must be declared at the beginning of functions, but they can be assigned.
* The variable returned by a function is called ret.

# Curly brackets

* Only functions opening curly brackets are on a new line.
* Curly brackets are optional for one line blocks.
* It is OK to put a goto in the same line of a if statement (in case of multiple if/goto).

# Best practices

* Use enum instead of defines when possible.

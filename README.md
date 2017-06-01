# `libwyzyrdry`

This is a library I use for personal items I find myself having to implement
often in C projects.

# Usage

Run `cmake .` and then `make`.

Run `bin/test` to run the tests as an example.

Add the artifact `target/libwyzyrdry.a` to your compilation search path and the
contents of `include/` to your include search path. Accomplish this however you
so choose.

# Components

The library will generally consist of "object-like" structures with associated
functions. The structures are named in PascalCase and functions in snake_case,
and associated functions have their structure's name (or abbreviation) as a
prefix.

"Instance method" functions always take a pointer to an instance of the struct
on which they operate as their first parameter.

## `Vec`

The `Vec` module describes a growable buffer on the heap. As C does not support
generics, it is byte-oriented.


# `libwyzyrdry`

This is a library I use for personal items I find myself having to implement
often in C projects.

# Usage

Run `cmake .` and then `make`.

Run `bin/test` to run the tests as an example.

Add the artifact `target/libwyzyrdry.a` to your compilation search path and the
contents of `include/` to your include search path. Accomplish this however you
so choose.

# Design

This library is a collection of *largely* self-contained modules, though items
which I find useful will be used throughout the library.

# Components

The library will generally consist of "object-like" structures with associated
functions. The structures are named in PascalCase and functions in snake_case,
and associated functions have their structure's name (or abbreviation) as a
prefix.

"Instance method" functions always take an instance of the struct on which they
operate as their first parameter.

Functions that mutate their "self" parameter take it as a pointer; functions
that do not mutate themselves take a copy (for small, stack items) or a const
pointer (for large or non-stack items).

## `Vec`

The `Vec` module describes a growable buffer on the heap. As C does not support
generics, it is byte-oriented.

## `Slice`

The `Slice` module is a pointer and length (in bytes). It can be used to
safely describe finite buffers and is essentially a formal version of the C
idiom of passing pointers and lengths as siblings, but not coupled in the type
system. Slices can be decomposed into their pointer and length components, but
cannot be considered ABI equivalent to a pointer and length as siblings. Slices
also support iterating over each byte in the buffer.

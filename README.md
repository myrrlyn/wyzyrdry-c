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
which I find useful will be used throughout the library.git log

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

## `Str`

The `Str` module is a length-prefixed buffer that can be used to serialize
variable-length items for transport through layers that may not know how to read
the type in question.

It uses a newtype wrapper over some unsigned integral type as its length and
index type. By default, this type is set to the 16-bit `unsigned short`, but can
be changed by modifying the `typedef unsigned <type> StrLen;` line in
`include/str.h`. If modified, this library must be recompiled for every codebase
that uses `Str` in the transport path.

## `Enum`

The `Enum` module is a header-only library that provides (somewhat) C-idiom
tagged unions, after the Rustic fashion. This uses preprocessor macros to create
new syntax, so there are some unpleasant restrictions.

A tagged-union type is declared with:

```c
ENUM(UnionName, vartype, varname, ...);
```

where the `vartype, varname` pairs repeat for up to thirteen total variants.
This is not a firm limit; it's just that C doesn't support recursive macros and
I got bored manually unrolling the macros. If more than thirteen variants are
needed, well, I'll expand that when I get there.

This macro declares two items: `typedef enum UnionNameTag {} UnionNameTag;` and
`typedef struct UnionName {} UnionName;` and populates them with the given
variants. The enum is filled with variants named `UnionName_varname`, the struct
has a union over all of the given `vartype` types, each named exactly `varname`
without any mangling, and a `UnionNameTag` discriminant.

Instances of the tagged-union are created with:

```c
UnionName inst = SET_VARIANT(UnionName, varname, body);
```

where `body` is an instance of the appropriate `vartype`. This can be a variable
or a literal.

The compiler can't typecheck these things, so keeping track of the discriminant
is a must.

The discriminant itself can be retrieved with:

```c
UnionNameTag disc = GET_VARIANT_TYPE(inst);
```

This has the unpleasant downside of leaking the name mangling strategy in use.
The `ENUM_TAG(UnionName)` macro will get the type name of the discriminant enum
for reducing information leaks.

The body of a tagged-union can be extracted with:

```c
vartype body = GET_VARIANT_BODY(inst, disc);
```

The actual type of `vartype` must, obviously, be known to the using code, so I
have not bothered creating a macro to extract the final type of the variant. I
also don't know how I would accomplish that.

### Example Usage

```c
//  Declare a Result enum that can be an Ok(void*) or an Err(char*)
ENUM(Result, void*, Ok, char*, Err);
Result r = SET_VARIANT(Result, Err, "Something went wrong");
switch (GET_VARIANT_TYPE(r)) {
case Result_Ok:
	void* inner = GET_VARIANT_BODY(r);
	break;
case Result_Err:
	char* msg = GET_VARIANT_BODY(r);
	printf("ERR: %s\n", msg);
	break;
default:
	//  This is unreachable.
}
```

## `RingBuf`

The `RingBuf` module provides a method of building circular buffers that hold
`Str` elements. The `RingBuf` is similar to the `Vec` in that it is a small
struct governing a discrete buffer of memory, which can be statically or
dynamically allocated.

`RingBuf` stores are guaranteed to provide `O(n)` access for push and pop
functions, where `n` is the size of the `Str` being moved, and to be correct
even when wrapping from the back of the store to the front.

As the buffer uses `Str` as its atom of storage, it will refuse to store data
that is larger than its remaining space available.

Methods are provided for receiving `Slice`, `Str`, and `Vec` objects. Storage of
other types should be done by creating a `Slice` descriptor and passing that in.

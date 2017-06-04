/**
 * The Str structure is used to serialize fixed-length buffers.
 *
 * As it is an unsized type, it can never be held directly, but rather lives as
 * a pointer into other memory. This memory can be given by malloc(), a static
 * buffer, or a Slice over some memory.
 */

#ifndef WYZYRDRY_STR_H
#define WYZYRDRY_STR_H

#include <slice.h>
#include <vec.h>

/**
 * Newtype for the length and index of a Str.
 *
 * Currently a u16 as a 64K Str "ought to be enough for anybody."
 */
typedef unsigned short StrLen;

typedef struct Str {
	StrLen len;
	unsigned char data[];
} Str;

Str* str_from_vec(Vec src);
Str* str_from_vec_in_place(Slice dst, Vec src);
Str* str_from_slice(Slice src);
Str* str_from_slice_in_place(Slice dst, Slice src);
void str_free(Str* self);

Slice str_as_slice(Str* self);

void str_debug_print(Str* self);

#endif
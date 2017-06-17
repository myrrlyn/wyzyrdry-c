/**
 * The Str structure is used to serialize fixed-length buffers.
 *
 * As it is an unsized type, it can never be held directly, but rather lives as
 * a pointer into other memory. This memory can be given by malloc(), a static
 * buffer, or a Slice over some memory.
 *
 * The in-memory representation of a Str is the StrLen length of the data, in
 * native endianness, followed by that many bytes of data. The length prefix
 * will need to be set to network endianness before transferring between
 * machines.
 */

#ifndef WYZYRDRY_STR_H
#define WYZYRDRY_STR_H

#include "slice.h"
#include "vec.h"

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

Str* str_from_vec(const Vec* const src);
Str* str_from_vec_in_place(const Slice dst, const Vec* const src);
Str* str_from_slice(const Slice src);
Str* str_from_slice_in_place(const Slice dst, const Slice src);
void str_free(Str* const self);


const Slice str_as_slice(Str* const self);

void str_debug_print(const Str* const self);

StrLen str_size(StrLen len);
StrLen str_capacity(StrLen size);

#endif

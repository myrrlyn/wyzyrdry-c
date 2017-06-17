/**
 * This module defines a Slice -- a length-tagged pointer to a buffer.
 *
 * Slices are useful in that they carry the length of their targeted buffer
 * alongside the pointer and do not rely on sentinel values for determining the
 * end of the buffer.
 */

#ifndef WYZYRDRY_SLICE_H
#define WYZYRDRY_SLICE_H

#include <stdlib.h>

typedef struct Slice {
	unsigned char* ptr;
	size_t len;
} Slice;

const Slice slice_new(unsigned char* const ptr, size_t len);
unsigned char* slice_ptr(const Slice self);
size_t slice_len(const Slice self);

void slice_for_each(const Slice self, void (*callback)(unsigned char c));

void slice_debug_print(const Slice self);

#endif

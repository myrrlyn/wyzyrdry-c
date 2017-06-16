/**
 * This module defines a Vec structure -- a growable, heap-allocated buffer of
 * bytes. Bytes must be added with `vec_push()`.
 */

#ifndef WYZYRDRY_VEC_H
#define WYZYRDRY_VEC_H

#include <stdlib.h>

#include "slice.h"

typedef struct Vec {
	unsigned char* buf;
	size_t len;
	size_t cap;
} Vec;

Vec vec_init(size_t capacity, size_t item_size);
void vec_free(Vec* const self);
void vec_push_byte(Vec* const self, unsigned char byte);
void vec_push_slice(Vec* const self, const Slice slice);
void vec_trim(Vec* const self);
Slice vec_as_slice(Vec self);

void vec_debug_print(Vec self);

#endif

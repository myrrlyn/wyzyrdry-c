#ifndef WYZYRDRY_VEC_H
#define WYZYRDRY_VEC_H

#include <stdlib.h>

typedef struct Vec {
	unsigned char* buf;
	size_t len;
	size_t cap;
} Vec;

int vec_init(Vec* self, size_t capacity, size_t item_size);
void vec_free(Vec* self);
void vec_push_byte(Vec* self, unsigned char byte);
void vec_trim(Vec* self);

#endif

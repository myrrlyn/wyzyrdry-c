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
int vec_push(Vec* self, unsigned char byte);
int vec_trim(Vec* self);

#endif

#ifndef WYZYRDRY_SLICE_H
#define WYZYRDRY_SLICE_H

#include <stdlib.h>

typedef struct Slice {
	unsigned char* ptr;
	size_t len;
} Slice;

Slice slice_new(unsigned char* ptr, size_t len);
unsigned char* slice_ptr(Slice* self);
size_t slice_len(Slice* self);

#endif

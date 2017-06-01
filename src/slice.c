#include <stdlib.h>

#include <slice.h>

/**
 * Make a new Slice out of a pointer and length.
 *
 * This presumes that the pointer is not null, and that the memory described by
 * the pointer and length is all valid.
 * @param ptr A pointer to valid memory.
 * @param len The length of memory captured by the slice.
 * @return A new Slice describing the memory.
 */
Slice slice_new(unsigned char* ptr, size_t len) {
	Slice ret = {
		.ptr = ptr,
		.len = len,
	};
	return ret;
}

/**
 * Accessor to the Slice's pointer.
 * @param self The Slice on which to act.
 * @return The Slice's pointer field.
 */
unsigned char* slice_ptr(Slice* self) {
	return self->ptr;
}

/**
 * Accessor to the slice's length.
 * @param self The Slice on which to act.
 * @return The Slice's length field.
 */
size_t slice_len(Slice* self) {
	return self->len;
}
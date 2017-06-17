#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <slice.h>
#include <vec.h>

void vec_realloc(Vec* self);

/**
 * Initialize a Vec structure.
 * @param capacity The starting capacity (in items) of the buffer.
 * @param item_size The size of items in the buffer.
 * @return A Vec structure. If malloc failed, buf will be NULL.
 */
Vec vec_init(size_t capacity, size_t item_size) {
	Vec ret = {
		.buf = NULL,
		.len = 0,
		.cap = 0,
	};
	size_t total = capacity * item_size;
	void* buf = malloc(total);
	if (buf == NULL) {
		return ret;
	}
	ret.buf = (unsigned char*)buf;
	ret.len = 0;
	ret.cap = total;
	return ret;
}

/**
 * Deallocate a Vec structure.
 *
 * This frees the buffer and sets len and cap to 0.
 * @param self The Vec on which to act.
 */
void vec_free(Vec* const self) {
	free(self->buf);
	self->buf = NULL;
	self->len = 0;
	self->cap = 0;
}

/**
 * Push a byte into the Vec.
 *
 * If reallocation occurs and fails, the pushed byte will be silently dropped.
 * A failed reallocation will cause the Vec to reset to empty state.
 * @param self The Vec on which to act.
 * @param byte The byte to push into the end of the Vec.
 */
void vec_push_byte(Vec* const self, unsigned char byte) {
	if (self->len == self->cap) {
		vec_realloc(self);
	}
	if (self->buf != NULL) {
		self->buf[self->len] = byte;
		++self->len;
	}
	else {
		self->len = 0;
		self->cap = 0;
	}
}

/**
 * Append a Slice to the Vec.
 * @param self The Vec on which to act.
 * @param slice The Slice to be appended into the Vec.
 */
void vec_push_slice(Vec* const self, const Slice slice) {
	for (size_t idx = 0; idx < slice.len; ++idx) {
		vec_push_byte(self, slice.ptr[idx]);
	}
}

/**
 * Trims the Vec's capacity to exactly match its current length.
 * @param self The Vec on which to act.
 */
void vec_trim(Vec* const self) {
	self->buf = realloc(self->buf, self->len);
	self->cap = self->len;
}

/**
 * Gets a reference to the interior contents of the Vec.
 * @param self The Vec on which to act.
 * @return A Slice (pointer and length) of the Vec's contents.
 */
Slice vec_as_slice(const Vec* const self) {
	return slice_new(self->buf, self->len);
}

/**
 * Print out the Vec for debugging purposes.
 * @param self The Vec on which to act.
 */
void vec_debug_print(const Vec* const self) {
	printf("Vec { buf: %p, len: %zu, cap: %zu }\nBuf: ", self->buf, self->len, self->cap);
	for (size_t idx = 0; idx < self->len; ++idx) {
		printf("%02X ", self->buf[idx]);
	}
	for (size_t idx = self->len; idx < self->cap; ++idx) {
		printf("__ ");
	}
	printf("\n");
}

/**
 * INTERNAL: Reallocate the Vec's buffer when it is full.
 *
 * This doubles the capacity.
 * @param self The Vec on which to act.
 */
void vec_realloc(Vec* self) {
	size_t newcap = 2 * self->cap;
	self->buf = (unsigned char*)realloc(self->buf, newcap);
	self->cap = newcap;
}

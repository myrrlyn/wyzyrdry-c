#include <string.h>
#include <vec.h>

void vec_realloc(Vec* self);

/**
 * Initialize a Vec structure.
 * @param self  The Vec on which to act.
 * @param capacity The starting capacity (in items) of the buffer.
 * @param item_size The size of items in the buffer.
 * @return -1 on allocation failure or 0 on success.
 */
int vec_init(Vec* self, size_t capacity, size_t item_size) {
	size_t total = capacity * item_size;
	void* buf = malloc(total);
	if (buf == NULL) {
		return -1;
	}
	self->buf = (unsigned char*)buf;
	self->len = 0;
	self->cap = total;
	return 0;
}

/**
 * Deallocate a Vec structure.
 *
 * This frees the buffer and sets len and cap to 0.
 * @param self The Vec on which to act.
 */
void vec_free(Vec* self) {
	free(self->buf);
	self->buf = NULL;
	self->len = 0;
	self->cap = 0;
}

/**
 * Push a byte into the Vec.
 * @param self The Vec on which to act.
 * @param byte The byte to push into the end of the Vec.
 */
void vec_push_byte(Vec* self, unsigned char byte) {
	if (self->len == self->cap) {
		vec_realloc(self);
	}
	self->buf[self->len] = byte;
	++self->len;
}

/**
 * Trims the Vec's capacity to exactly match its current length.
 * @param self The Vec on which to act.
 */
void vec_trim(Vec* self) {
	self->buf = realloc(self->buf, self->len);
	self->cap = self->len;
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

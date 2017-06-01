#include <string.h>
#include <vec.h>

void vec_realloc(Vec* self);

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

void vec_free(Vec* self) {
	free(self->buf);
	self->buf = NULL;
	self->len = 0;
	self->cap = 0;
}

int vec_push(Vec* self, unsigned char byte) {
	if (self->len == self->cap) {
		vec_realloc(self);
	}
	self->buf[self->len] = byte;
	++self->len;
}

int vec_trim(Vec* self) {
	self->buf = realloc(self->buf, self->len);
	self->cap = self->len;
	return 0;
}

void vec_realloc(Vec* self) {
	size_t newcap = 2 * self->cap;
	self->buf = (unsigned char*)realloc(self->buf, newcap);
	self->cap = newcap;
}

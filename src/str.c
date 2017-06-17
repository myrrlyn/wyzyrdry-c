#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wyzyrdry.h>

Str* str_new(StrLen len);

/**
 * Copy a Vec into a newly allocated Str buffer.
 * @param src The Vec whose contents will be written into the new Str.
 * @return A pointer to the newly allocated Str buffer.
 */
Str* str_from_vec(const Vec* const src) {
	Str* ret = str_new((StrLen)src->len);
	if (ret != NULL) {
		memmove(&ret->data, src->buf, src->len);
		ret->len = (StrLen)src->len;
	}
	return ret;
}

/**
 * Copy a Vec into a pre-existing Slice buffer.
 *
 * This mutates the buffer the slice describes, but does not alter the Slice
 * descriptor itself.
 *
 * If the target Slice cannot receive the Vec-as-a-Str in entirety, then this
 * function returns NULL.
 * @param dst A Slice describing the buffer into which the Vec will be written
 * in the form of a Str.
 * @param src The Vec whose contents will be written into the Str.
 * @return A pointer to the Str resulting from this operation. This pointer is
 * identical to dst.ptr
 */
Str* str_from_vec_in_place(const Slice dst, const Vec* const src) {
	/*
	 * Check if the destination can receive the source as a Str. If not, exit.
	 */
	if (dst.len < str_size((StrLen)src->len)) {
		return NULL;
	}

	Str* ret = (void*)dst.ptr;
	ret->len = (StrLen)src->len;
	memmove(&ret->data, src->buf, src->len);
	return ret;
}

/**
 * Copy a Slice into a newly allocated Str buffer.
 * @param src The Slice whose contents will be written into the new Str.
 * @return A pointer to the newly allocated Str buffer.
 */
Str* str_from_slice(const Slice src) {
	Str* ret = str_new(src.len);
	if (ret != NULL) {
		memmove(&ret->data, src.ptr, src.len);
		ret->len = (StrLen)src.len;
	}
	return ret;
}

/**
 * Copy a slice of data into another pre-existing Slice.
 *
 * The return value Str* will be identical to the pointer in dst.ptr. If the
 * destination buffer is smaller than the source, this function returns NULL.
 * @param dst The destination Slice into which the source Slice will be written
 * as a Str.
 * @param src The source Slice whose data will be stored as a Str in the
 * destination.
 * @return A pointer to the destination Slice's buffer, which now holds a Str.
 */
Str* str_from_slice_in_place(const Slice dst, const Slice src) {
	/*
	 * Check if the destination can receive the source as a Str. If not, exit.
	 */
	if (dst.len < str_size((StrLen)src.len)) {
		return NULL;
	}

	Str* ret = (void*)dst.ptr;
	ret->len = (StrLen)src.len;
	memmove(&ret->data, src.ptr, src.len);
	return ret;
}

/**
 * Deallocate a Str.
 * @param self The Str to deallocate.
 */
void str_free(Str* self) {
	memset(self->data, 0, self->len);
	self->len = 0;
	free(self);
}

/**
 * Get a Slice over a Str's data payload.
 * @param self The Str to be described as a Slice.
 * @return A Slice describing the Str.
 */
const Slice str_as_slice(Str* const self) {
	return (Slice){
		.ptr = self->data,
		.len = self->len,
	};
}

/**
 * Print a Str for debugging purposes. This prints the high-level view similar
 * to Vec and Slice prints, the data payload in hex, and then the entire Str's
 * contents (length prefix and data)
 * @param self
 */
void str_debug_print(const Str* const self) {
	printf("Str { len: %zu, data ... }\nData: ", (size_t)self->len);
	hex_print(str_as_slice((Str* const)self));
	printf("RawStr: ");
	hex_print(slice_new((unsigned char*)self, str_size(self->len)));
}

/**
 * Allocate a buffer for a new Str
 * @param len The data count that the new Str will be able to hold.
 * @return A pointer to a new Str region.
 */
Str* str_new(StrLen len) {
	return malloc(str_size(len));
}

/**
 * INTERNAL USE: Get the size of a Str that wraps a certain amount of data.
 * @param len The amount of data to wrap with a Str.
 * @return The size of the Str that will wrap the given data.
 */
StrLen str_size(StrLen len) {
	return len + sizeof(StrLen);
}

/**
 * INTERNAL USE: Get the data capacity of a Str.
 * @param size The length of a data buffer.
 * @return The amount of data that buffer can hold formatted as a Str.
 */
StrLen str_capacity(StrLen size) {
	if (size > sizeof(StrLen)) {
		return size - sizeof(StrLen);
	}
	else {
		return 0;
	}
}

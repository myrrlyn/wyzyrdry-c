#include <stdio.h>
#include <stdlib.h>

#include <slice.h>
#include <wyzyrdry.h>

/**
 * Make a new Slice out of a pointer and length.
 *
 * This presumes that the pointer is not null, and that the memory described by
 * the pointer and length is all valid.
 * @param ptr A pointer to valid memory.
 * @param len The length of memory captured by the slice.
 * @return A new Slice describing the memory.
 */
const Slice slice_new(unsigned char* const ptr, size_t len) {
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
unsigned char* slice_ptr(const Slice self) {
	return self.ptr;
}

/**
 * Accessor to the slice's length.
 * @param self The Slice on which to act.
 * @return The Slice's length field.
 */
size_t slice_len(const Slice self) {
	return self.len;
}

/**
 * Invoke a function on each byte in the slice.
 * @param self The Slice over which to loop.
 * @param callback The function to be invoked with each byte in the Slice.
 */
void slice_for_each(const Slice self, void (*callback)(unsigned char c)) {
	for (size_t idx = 0; idx < self.len; ++idx) {
		callback(self.ptr[idx]);
	}
}

/**
 * Print out the Slice for debugging purposes.
 * @param self The Slice on which to act.
 */
void slice_debug_print(const Slice self) {
	printf("Slice { ptr: %p, len: %zu }\nPtr: ", self.ptr, self.len);
	hex_print(self);
}

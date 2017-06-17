#ifndef WYZYRDRY_MQRB_H
#define WYZYRDRY_MQRB_H

#include <stdlib.h>

#include "enum.h"
#include "slice.h"
#include "str.h"
#include "vec.h"

/**
 * A control structure governing some `Slice` of memory to be treated as a
 * circular FIFO queue.
 *
 * `RingBuf`s store their contents as `Str`s.
 */
typedef struct RingBuf {
	/**
	 * The `Slice` in which the `RingBuf`'s contents are stored.
	 */
	Slice store;
	/**
	 * The index of the start of the first (oldest) item in the queue.
	 */
	size_t head;
	/**
	 * The index of the first empty byte after the last (newest) item in the
	 * queue ends.
	 */
	size_t tail;
	/**
	 * The count of how many items are currently stored in the queue.
	 */
	size_t count;
} RingBuf;

RingBuf ringbuf_init(const Slice store);
void ringbuf_free(RingBuf* const self);
void ringbuf_wipe(RingBuf* const self);

size_t ringbuf_space_free(const RingBuf self);
size_t ringbuf_space_used(const RingBuf self);
StrLen ringbuf_peek_len(RingBuf self);

StrLen ringbuf_write_slice(RingBuf* const self, const Slice in);
StrLen ringbuf_write_str(RingBuf* const self, const Str* const in);
StrLen ringbuf_write_vec(RingBuf* const self, const Vec in);
StrLen ringbuf_read(RingBuf* const self, const Slice out);

void ringbuf_pop(RingBuf* const self);

void ringbuf_debug_print(RingBuf self);

#endif

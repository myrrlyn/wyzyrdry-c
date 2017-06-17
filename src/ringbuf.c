#include <stdio.h>
#include <string.h>
#include <ringbuf.h>
#include <wyzyrdry.h>

/**
 * Structure used to indicate that a transaction on the RingBuf will require
 * wrapping around the end of the Slice.
 *
 * The naming convention is a bit screwy -- the bytes indicated by the `front`
 * member are at the back end of the storage Slice, while the bytes indicated by
 * the `back` member are at the front of the storage Slice.
 */
typedef struct RbWrap {
	/**
	 * The number of bytes before the wrap.
	 */
	StrLen front;
	/**
	 * The number of bytes after the wrap.
	 */
	StrLen back;
} RbWrap;

/**
 * Types of messages that may be generated from checking potential transactions.
 *
 * The NoOp variant indicates that the desired transaction cannot occur, with an
 * explanatory string.
 *
 * The NoWrap variant indicates that the desired transaction can occur without
 * requiring a wrap around the end of the store. It carries the total length of
 * the transaction can occur.
 *
 * The Wrap variant indicates that the desired transaction can occur, but will
 * require a wrap around the end of the store. It carries the number of bytes
 * before and after the wrap to complete the transaction.
 */
ENUM(RbAct, char*, NoOp, StrLen, NoWrap, RbWrap, Wrap);

/**
 * Types of operations that may be done on a `RingBuf`.
 */
ENUM(RbOp, StrLen, Read, StrLen, Write);

StrLen ringbuf_push_raw(
	RingBuf* const self,
	StrLen len,
	const unsigned char* const src
);
RbAct ringbuf_check(RingBuf self, RbOp op);

/**
 * Initialize a `RingBuf` over the given `Slice` of memory.
 * @param store A `Slice` describing the memory to be used as the `RingBuf`'s
 * store.
 * @return The `RingBuf` control structure governing the given Slice.
 */
RingBuf ringbuf_init(const Slice store) {
	RingBuf ret = {
		.store = store,
	};
	ringbuf_wipe(&ret);
	return ret;
}

/**
 * Deallocate the memory behind a `RingBuf` and erase its control structure.
 * @param self A pointer to the `RingBuf` to be erased.
 */
void ringbuf_free(RingBuf* const self) {
	ringbuf_wipe(self);
	Slice old = self->store;
	self->store = slice_new(NULL, 0);
	free(old.ptr);
}

/**
 * Erase the contents of a `RingBuf` backing store.
 * @param self The `RingBuf` to be erased.
 */
void ringbuf_wipe(RingBuf* const self) {
	memset(self->store.ptr, 0, self->store.len);
	self->head = 0;
	self->tail = 0;
	self->count = 0;
}

/**
 * Calculates how many bytes of the store are not in active use.
 * @param self The `RingBuf` to inspect.
 * @return A count of available bytes.
 */
size_t ringbuf_space_free(RingBuf self) {
	return self.store.len - ringbuf_space_used(self);
}

/**
 * Calculates how many bytes of the store are in active use.
 * @param self The `RingBuf` to inspect.
 * @return A count of unavailable bytes.
 */
size_t ringbuf_space_used(RingBuf self) {
	if (self.count == 0) {
		return 0;
	}
	if (self.head < self.tail) {
		return self.tail - self.head;
	}
	else {
		return self.store.len - self.head + self.tail;
	}
}

/**
 * Retrieves the length of the first Str in the queue, if any.
 * @param self The `RingBuf` on which to act.
 * @return The length of the first `Str` in the queue, or zero if empty.
 */
StrLen ringbuf_peek_len(RingBuf self) {
	if (self.count == 0) {
		return 0;
	}
	else {
		RbOp op = SET_VARIANT(RbOp, Read, 0);
		RbAct rbt = ringbuf_check(self, op);
		switch (GET_VARIANT_TYPE(rbt)) {
			/* This function doesn't need the error message */
			case ENUM_VAR(RbAct, NoOp): {
				return 0;
			}
			case ENUM_VAR(RbAct, NoWrap): {
				/* Treat self.head as indexing to a StrLen, deref, and return */
				return *(StrLen*)&self.store.ptr[self.head];
			}
			case ENUM_VAR(RbAct, Wrap): {
				/* Collect the wrap information about the prefix. */
				RbWrap w = GET_VARIANT_BODY(rbt, Wrap);
				/* Prep to receive a StrLen */
				unsigned char tmp[sizeof(StrLen)];
				/* Move `front` bytes from self.head into tmp */
				memmove(tmp, &self.store.ptr[self.head], w.front);
				/* Move `back` bytes from self[0] into tmp[front] */
				memmove(&tmp[w.front], self.store.ptr, w.back);
				/* tmp is now the bytes of a StrLen; cast, deref, and return. */
				return *(StrLen*)tmp;
			}
			default:
				return 0;
		}
	}
}

/**
 * Pushes a `Str*` into the RingBuf, if possible.
 * @param self The `RingBuf` into which the `Str*` is pushed.
 * @param in The `Str*` to be pushed.
 * @return The total number of bytes added to the queue.
 */
StrLen ringbuf_write_str(RingBuf* const self, const Str* const in) {
	return ringbuf_push_raw(self, (StrLen)in->len, in->data);
}

/**
 * Pushes a `Vec`'s contents into the queue.
 * @param self The queue to receive the Vec.
 * @param in The `Vec` to be pushed.
 * @return The amount of data pushed into the queue.
 */
StrLen ringbuf_write_vec(RingBuf* const self, const Vec in) {
	return ringbuf_push_raw(self, (StrLen)in.len, in.buf);
}

/**
 * Pushes a `Slice`'s contents into the queue.
 * @param self The queue to receive the `Slice`.
 * @param in The `Slice` to be pushed.
 * @return The amount of data pushed into the queue.
 */
StrLen ringbuf_write_slice(RingBuf* const self, const Slice in) {
	return ringbuf_push_raw(self, (StrLen)in.len, in.ptr);
}

/**
 * Moves the first message out of the queue, if it is not empty.
 *
 * If the `Slice` destination parameter cannot hold the first message, then the
 * transaction aborts without mutating the queue. If the destination can hold
 * the first message, then the body of the first `Str` is moved into the
 * `Slice`, and the number of message bytes moved is returned.
 * @param self The `RingBuf` from which to attempt a pop.
 * @param out The `Slice` into which the message (if any) will be delivered.
 * @return The number of bytes moved.
 */
StrLen ringbuf_read(RingBuf* const self, const Slice out) {
	StrLen msglen = ringbuf_peek_len(*self);
	/* Abort if there is no message, or the message is too large to fit. */
	if (msglen == 0 || msglen > out.len) {
		return 0;
	}
	/* Check what behavior is required to extract the message */
	RbAct rba = ringbuf_check(*self, SET_VARIANT(RbOp, Read, msglen));
	switch (GET_VARIANT_TYPE(rba)) {
		case ENUM_VAR(RbAct, NoWrap): {
			Str* msg = (Str*)&self->store.ptr[self->head];
			memmove(out.ptr, &msg->data, msglen);
			self->head += GET_VARIANT_BODY(rba, NoWrap);
			break;
		}
		case ENUM_VAR(RbAct, Wrap): {
			RbWrap w = GET_VARIANT_BODY(rba, Wrap);
			/* Check if the wrap occurred in the prefix */
			if (w.front <= sizeof(StrLen)) {
				/* set the head at the start of the data (near index 0) */
				self->head = sizeof(StrLen) - w.front;
				memmove(out.ptr, &self->store.ptr[self->head], msglen);
				self->head += msglen;
			}
			/* Otherwise, it occurred in the data */
			else {
				/* adjust the front and head to skip the prefix */
				w.front -= sizeof(StrLen);
				self->head += sizeof(StrLen);
				memmove(out.ptr, &self->store.ptr[self->head], w.front);
				memmove(&out.ptr[w.front], self->store.ptr, w.back);
				self->head = w.back;
			}
			break;
		}
		default:
			return 0;
	}
	self->count--;
	/* If the queue is empty, set both cursors to the start of the store */
	if (self->count == 0) {
		self->head = 0;
		self->tail = 0;
	}
	return msglen;
}

/**
 * Unconditionally destroys the first message in the queue.
 *
 * This sets the head cursor to target the cell immediately after the first
 * message ends. If the queue is empty, this function aborts without acting.
 * @param self
 */
void ringbuf_pop(RingBuf* const self) {
	StrLen msglen = ringbuf_peek_len(*self);
	if (msglen == 0) {
		return;
	}
	RbAct rba = ringbuf_check(*self, SET_VARIANT(RbOp, Read, msglen));
	switch (GET_VARIANT_TYPE(rba)) {
		/* The first stored message does not wrap; bump head */
		case ENUM_VAR(RbAct, NoWrap): {
			self->head += GET_VARIANT_BODY(rba, NoWrap);
			break;
		}
		/* The first stored message does wrap; set head */
		case ENUM_VAR(RbAct, Wrap): {
			RbWrap w = GET_VARIANT_BODY(rba, Wrap);
			self->head = w.back;
			break;
		}
		default:
			return;
	}
	/*
	 * Since no data is being erased from the store, the count must decrement or
	 * the queue will enter into an invalid state.
	 */
	self->count--;
	/* If the queue is empty, set both cursors to the start of the store */
	if (self->count == 0) {
		self->head = 0;
		self->tail = 0;
	}
}

/**
 * Display the `RingBuf` for debugging purposes.
 * @param self
 */
void ringbuf_debug_print(RingBuf self) {
	printf(
		"RingBuf { store: { ptr: %p, cap: %zu }, head: %zu, tail: %zu, count: %zu }\n",
		(void*)self.store.ptr,
		self.store.len,
		(size_t)self.head,
		(size_t)self.tail,
		(size_t)self.count
	);
	if (self.store.ptr != NULL) {
		printf("Contents: ");
		hex_print(slice_new(self.store.ptr, 32));
	}
}

/**
 * Base function for pushing data into the queue.
 *
 * The type dispatch functions just disassemble their inputs into a length and a
 * buffer, then pass those pieces on to this function for the actual work.
 *
 * If this were a language with interfaces or traits, then this library could
 * just require an interface like RingBufPushable with methods get_len() and
 * get_buf(), and any type that wanted to be pushed into a RingBuf would
 * implement them and the compiler or runtime would connect the pieces. But C is
 * not such a language.
 * @param self The `RingBuf` into which the data is being pushed.
 * @param len The amount of data to be pushed.
 * @param src The source of data to be pushed.
 * @return The amount of data actually pushed into the queue, including the
 * length prefix.
 */
StrLen ringbuf_push_raw(
	RingBuf* const self,
	StrLen len,
	const unsigned char* const src
) {
	/* If the queue is empty, set both cursors to the start of the store */
	if (self->count == 0) {
		self->head = 0;
		self->tail = 0;
	}
	/* Get the total size of the data to be pushed into the queue's store */
	StrLen strlen = str_size(len);
	/* Check if the queue can receive that much data */
	RbAct rba = ringbuf_check(*self, SET_VARIANT(RbOp, Write, len));
	switch (GET_VARIANT_TYPE(rba)) {
		/* It can't */
		case ENUM_VAR(RbAct, NoOp): {
			fprintf(stderr, "%s\n", GET_VARIANT_BODY(rba, NoOp));
			return 0;
		}
		/* It can, easily */
		case ENUM_VAR(RbAct, NoWrap): {
			/* Write the length prefix into the current tail */
			*(StrLen*)&self->store.ptr[self->tail] = len;
			self->tail += sizeof(StrLen);
			/* Write the data into the updated tail */
			memmove(&self->store.ptr[self->tail], src, len);
			self->tail += len;
			break;
		}
		/* It can, less easily */
		case ENUM_VAR(RbAct, Wrap): {
			RbWrap w = GET_VARIANT_BODY(rba, Wrap);
			/* Check if the wrap occurs in the middle of the length header */
			if (w.front <= sizeof(StrLen)) {
				/* Pretend the length is a byte buffer, and push it */
				unsigned char* tmp = (void*)&len;
				memmove(&self->store.ptr[self->tail], tmp, w.front);
				unsigned char tback = sizeof(StrLen) - w.front;
				memmove(self->store.ptr, &tmp[w.front], tback);
				self->tail = tback;
				/* Push the data */
				memmove(&self->store.ptr[self->tail], src, len);
				self->tail += len;
				break;
			}
			else {
				/* Push the length */
				*(StrLen*)&self->store.ptr[self->tail] = len;
				self->tail += sizeof(StrLen);
				/* Update the wrap state to account for the pushed length */
				w.front -= sizeof(StrLen);
				/* Push the data */
				memmove(&self->store.ptr[self->tail], src, w.front);
				memmove(self->store.ptr, &src[w.front], w.back);
				self->tail = w.back;
				break;
			}
		}
		default:
			return 0;
	}
	self->count++;
	return strlen;
}

/**
 * Preflight buffer inspection for transactions. This function must be called to
 * determine the state of health of the buffer before attempting to act on it.
 *
 * The `Rbo_Read` and `Rbo_Write` operations provide information about how the
 * transaction will occur. A return of NoOp means that the queue is unable to
 * support it, either empty or too full to transfer a Str of the given size.
 * NoWrap means that the transaction can occur in one memmove starting at the
 * appropriate cursor. Wrap(f, b) means that the transaction can occur, but will
 * require two memmoves, one at the appropriate cursor, and the other at the
 * start of the store.
 *
 * The `NoWrap` and `Wrap` return variants carry the size of the entire `Str`,
 * not just of the payload of that `Str`.
 * @param self The `RingBuf` whose store will be checked.
 * @param op A Read or Write operation seeking to move an amount of data. The
 * interior value does not include the Str header.
 * @return An `RbAct` variant describing the operation.
 */
RbAct ringbuf_check(RingBuf self, RbOp op) {
	/* RBO variants are the same type, so just extract the body */
	StrLen msglen = GET_VARIANT_BODY(op, Read);
	/* Add the Str heading */
	StrLen strlen = str_size(msglen);
	switch (GET_VARIANT_TYPE(op)) {
		case ENUM_VAR(RbOp, Read): {
			/* Early failure if empty queues can't support a read operation. */
			if (self.count == 0) {
				return SET_VARIANT(RbAct, NoOp, "The queue is empty");
			}
			/* Get the number of bytes between head and end-of-store */
			StrLen back = (StrLen)(self.store.len - self.head);
			/* If there are at least as many store bytes as transfer, NoWrap. */
			if (back >= strlen) {
				return SET_VARIANT(RbAct, NoWrap, strlen);
			}
			/*
			 * Otherwise, it wraps with the back bytes in the front of the data
			 * and start-of-store to end-of-data in the back of the data.
			 */
			return SET_VARIANT(RbAct, Wrap, ((RbWrap){
				.front = back,
				.back = strlen - back,
			}));
		}
		case ENUM_VAR(RbOp, Write): {
			/*
			 * Check if the queue has enough space to accommodate a Str of the
			 * requested payload size.
			 */
			if (ringbuf_space_free(self) < strlen) {
				return SET_VARIANT(RbAct, NoOp, "Message too large!");
			}
			/* Same logic as above, but for tail instead of head */
			StrLen back = (StrLen)(self.store.len - self.tail);
			if (back >= strlen) {
				return SET_VARIANT(RbAct, NoWrap, strlen);
			}
			return SET_VARIANT(RbAct, Wrap, ((RbWrap){
				.front = back,
				.back = strlen - back,
			}));
		}
	}
	/* Unreachable. */
	return SET_VARIANT(RbAct, NoOp, "CONTROL FLOW ERROR!");
}

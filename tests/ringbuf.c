#include <stdio.h>
#include <wyzyrdry.h>

void test_ringbuf() {
	size_t sz = 32;
	void* buf = malloc(sz);
	Slice s = slice_new(buf, sz);
	RingBuf rb = ringbuf_init(s);

	printf("\nExpectation: RingBuf exists, and has zeroed cursors.\n");
	ringbuf_debug_print(&rb);

	printf("\nExpectation: RingBuf is empty.\n");
	printf(
		"Space used: %zu, space free: %zu.\n",
		(size_t)ringbuf_space_used(&rb),
		(size_t)ringbuf_space_free(&rb)
	);

	Slice greet = slice_new((unsigned char*)"Hello, world!", 13);
	Str* s0 = str_from_slice(greet);
	str_debug_print(s0);
	ringbuf_write_str(&rb, s0);
	printf("\nExpectation: RingBuf contains a Str of 'Hello, world!'\n");
	ringbuf_debug_print(&rb);

	greet = slice_new((unsigned char*)"Saluton, mondo!", 15);
	Vec v0 = vec_init(16, 1);
	vec_push_slice(&v0, greet);
	ringbuf_write_vec(&rb, &v0);
	vec_free(&v0);
	printf("\nExpectation: RingBuf contains another Str, of 'Saluton, mondo!'\n");
	ringbuf_debug_print(&rb);

	v0 = vec_init(16, 1);
	Slice vs0 = slice_new(v0.buf, v0.cap);
	v0.len = ringbuf_read(&rb, vs0);
	printf("\nExpectation: Vec contains 'Hello, world!', RingBuf holds both in memory but only one in control.\n");
	vec_debug_print(&v0);
	ringbuf_debug_print(&rb);
	v0.len = ringbuf_read(&rb, vs0);
	printf("\nExpectation: Vec contains 'Saluton, mondo!', RingBuf holds both in memory and none in control.\n");
	vec_debug_print(&v0);
	ringbuf_debug_print(&rb);

	ringbuf_wipe(&rb);
	printf("\nExpectation: RingBuf is cleaned and ready to receive again.\n");
	ringbuf_debug_print(&rb);

	printf("\nSaturate the buffer and check wrapping, overlay behavior.\n");
	Slice foo = slice_new((unsigned char*)"abcde", 5);
	ringbuf_write_slice(&rb, foo);
	ringbuf_write_slice(&rb, foo);

	for (size_t idx = 0; idx < 10; ++idx) {
		ringbuf_write_slice(&rb, foo);
		v0.len = ringbuf_read(&rb, vs0);
		ringbuf_debug_print(&rb);
	}
	printf("The strings should have wrapped, and have visibly written new over old.\n");

	/*
	 * Free the memory through the RingBuf destructor.
	 */
	ringbuf_free(&rb);
	printf("\nExpectation: RingBuf is zeroed and freed.\n");
	ringbuf_debug_print(&rb);
}
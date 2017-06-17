/**
 * Tests for the Str module
 */

#include <stdio.h>
#include <string.h>
#include <wyzyrdry.h>

void test_str(void) {
	Vec vec = vec_init(16, 1);
	Slice greet = slice_new((unsigned char*)"Saluton, mondo!\n", 16);
	vec_push_slice(&vec, greet);

	Str* vec1 = str_from_vec(&vec);
	printf("\nExpectation: A new Str is formed, which has the same length as the Vec.\n");
	printf("Str address: %p, Vec buffer address: %p.\n", vec1, vec.buf);
	printf("Str len: %zu, Vec len: %zu.\n", (size_t)vec1->len, vec.len);

	printf("\nExpectation: Str has the same data contents as the Vec.\n");
	str_debug_print(vec1);
	printf("----\n");
	vec_debug_print(&vec);

	Str* slice1 = str_from_slice(greet);
	printf("\nExpectation: A new Str is formed, with the same length and content as the Slice.\n");
	printf("Str ptr/len: %p, %zu.\n", slice1, (size_t)slice1->len);
	slice_debug_print(greet);
	str_debug_print(slice1);

	/*
	 * Note: Ownership of the memory is going to get WEIRD. This is a scenario
	 * where Rust would offer significant help.
	 *
	 * First up, we pull a Vec from the heap. This invokes malloc, and the Vec
	 * owns the buffer malloc provides.
	 */
	Vec vec_in_place = vec_init(32, 1);
	printf("\nExpectation: A new Vec with capacity 32 exists.\n");
	vec_debug_print(&vec_in_place);
	Slice empty = slice_new((unsigned char*)"                                ", 32);
	vec_push_slice(&vec_in_place, empty);
	/*
	 * The Vec is now described as a Slice. A Slice cannot own memory; it can
	 * only describe other regions. As such, Slice has no alloc or free methods,
	 * and is not considered to OWN the heap memory in question, merely borrow
	 * it.
	 */
	Slice vip_slice = vec_as_slice(&vec_in_place);
	printf("Expectation: A Slice describing that Vec exists.\n");
	slice_debug_print(vip_slice);
	/*
	 * We now create a Str* to the buffer the Vec governs, and populate it with
	 * the contents of another Slice.
	 *
	 * The Str pointer returned from this operation is the Vec.buf member. In a
	 * better language like Rust, this would cause the Vec to go out of scope,
	 * and transfer the buffer to the Str, so that when the Str is destroyed the
	 * memory is released.
	 *
	 * Anyway, Vec.buf should now be [16 0 'S' 'a' 'l' 'u' 't' 'o' 'n' ...]
	 */
	Str* str_in_vip = str_from_slice_in_place(vip_slice, greet);
	printf("Expectation: A Str exists.\n");
	str_debug_print(str_in_vip);

	printf("\nExpectation: The Str pointer is the Vec.buf member.\n");
	printf("Str: %p, Vec.buf: %p.\n", str_in_vip, vec_in_place.buf);

	/*
	 * ONLY ONE OBJECT WITH ACCESS TO THE HEAP MEMORY CAN DEALLOCATE.
	 *
	 * Slice does not have a built-in deallocator; it must be presumed invalid
	 * after this occurs but will not perform any cleanup.
	 *
	 * This is a Str test, so we will deallocate through the Str. The Vec will
	 * become dangling, and must be left unused until it goes out of scope.
	 */
	str_free(str_in_vip);

	printf("\nExpectation: The Str has been zeroed during deallocation.\n");
	printf("This test may cause issues, as it is reading memory used after free.\n");
	hex_print(vip_slice);
}

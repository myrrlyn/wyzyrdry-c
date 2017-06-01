#include <lib.h>


void test_vec(void) {
	Vec vec;
	vec_init(&vec, 16, sizeof(unsigned char));
	printf("\nExpectation: Vec has a valid pointer, length 0, capacity 16\n");
	printf("Vec { buf: %p, len: %i, cap: %i }\n", vec.buf, vec.len, vec.cap);

	char greet[] = "Saluton, mondo!";
	for (size_t idx = 0; idx < strlen(greet); ++idx) {
		vec_push(&vec, greet[idx]);
	}
	printf("\nExpectation: Vec has the same pointer, length 15, and buf \"Saluton, mondo!\"\n");
	printf("Vec { buf: %p, len: %i, cap: %i }\n", vec.buf, vec.len, vec.cap);
	printf("Buf: %s\n", vec.buf);

	for (size_t idx = 0; idx < strlen(greet); ++idx) {
		vec_push(&vec, greet[idx]);
	}
	printf("\nExpectation: Vec has a new pointer, length 30, capacity 32, and buf \"Saluton, mondo!Saluton, mondo!\"\n");
	printf("Vec { buf: %p, len: %i, cap: %i }\n", vec.buf, vec.len, vec.cap);
	printf("Buf: %s\n", vec.buf);

	vec_trim(&vec);
	printf("\nExpectation: Vec has capacity 30.\n");
	printf("Vec { buf: %p, len: %i, cap: %i }\n", vec.buf, vec.len, vec.cap);

	vec_free(&vec);
	printf("\nExpectation: Vec has buf nil, len 0, cap 0.\n");
	printf("Vec { buf: %p, len: %i, cap: %i }\n", vec.buf, vec.len, vec.cap);
}

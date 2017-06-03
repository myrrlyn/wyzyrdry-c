#include <stdio.h>
#include <string.h>
#include <wyzyrdry.h>

void test_vec(void) {
	Vec vec = vec_init(16, sizeof(unsigned char));
	printf("\nExpectation: Vec has a valid pointer, length 0, capacity 16\n");
	vec_debug_print(vec);

	char greet[] = "Saluton, mondo!";
	for (size_t idx = 0; idx < strlen(greet); ++idx) {
		vec_push_byte(&vec, greet[idx]);
	}
	printf("\nExpectation: Vec has the same pointer, length 15, and buf \"Saluton, mondo!\"\n");
	vec_debug_print(vec);

	Slice svec = vec_as_slice(vec);
	printf("\nExpectation: A Slice to the Vec's buf and length.\n");
	slice_debug_print(svec);

	for (size_t idx = 0; idx < strlen(greet); ++idx) {
		vec_push_byte(&vec, greet[idx]);
	}
	printf("\nExpectation: Vec has a new pointer, length 30, capacity 32, and buf \"Saluton, mondo!Saluton, mondo!\"\n");
	vec_debug_print(vec);

	vec_trim(&vec);
	printf("\nExpectation: Vec has capacity 30.\n");
	vec_debug_print(vec);

	vec_free(&vec);
	printf("\nExpectation: Vec has buf nil, len 0, cap 0.\n");
	vec_debug_print(vec);
}

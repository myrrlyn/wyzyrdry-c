#include <stdio.h>
#include <string.h>
#include <wyzyrdry.h>

char greet[] = "Saluton, mondo!\n";

void print_char_as_text(unsigned char c) {
	printf("%c", c);
}

void test_slice() {
	Slice slice = slice_new((unsigned char*)greet, strlen(greet));

	unsigned char* sptr = slice_ptr(slice);
	size_t slen = slice_len(slice);
	printf("\nExpectation: Slice ptr/len equal to greet ptr/len\n");
	printf("Source ptr/len: %p %zu\n", greet, strlen(greet));
	slice_debug_print(slice);

	printf("\nExpectation: Iterate over the slice to print it as text.\n");
	slice_for_each(slice, print_char_as_text);
}

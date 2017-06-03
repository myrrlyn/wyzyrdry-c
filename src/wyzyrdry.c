/**
 * This will likely remain eternally empty, though it may accumulate free
 * functions that do not require their own module.
 */

#include <stdio.h>

#include <wyzyrdry.h>

/**
 * Print out a Slice with its printable characters as ASCII and unprintable as
 * hexadecimal.
 * @param item The Slice to print out.
 */
void hex_print(Slice item) {
	for (size_t idx = 0; idx < item.len; ++idx) {
		printf("%02X ", item.ptr[idx]);
	}
	for (size_t idx = 0; idx < item.len; ++idx) {
		unsigned char c = item.ptr[idx];
		if (c >= ' ') {
			printf("%c", c);
		}
		else {
			printf(".");
		}
	}
	printf("\n");
}

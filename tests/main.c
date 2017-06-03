#include <stdio.h>

void test_slice(void);
void test_vec(void);

int main(int argc, char* argv[]) {
	printf("Testing Vec!\n");
	test_vec();
	printf("\nTesting Slice!\n");
	test_slice();
}

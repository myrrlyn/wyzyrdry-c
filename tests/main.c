#include <stdio.h>

void test_enum(void);
void test_ringbuf(void);
void test_slice(void);
void test_str(void);
void test_vec(void);

int main(int argc, char* argv[]) {
	printf("Testing Vec!\n");
	test_vec();
	printf("\nTesting Slice!\n");
	test_slice();
	printf("\nTesting Str!\n");
	test_str();
	printf("\nTesting Enum!\n");
	test_enum();
	printf("\nTesting Ringbuf!\n");
	test_ringbuf();
}

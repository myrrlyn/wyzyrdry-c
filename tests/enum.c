#include <stdio.h>

#include <enum.h>

typedef struct EnumTestObj {
	int a;
	int b;
} EnumTestObj;

ENUM(EnumTest, char*, One, int, Two, EnumTestObj, Three);

EnumTest var = SET_VARIANT(EnumTest, One, "Hello, world!");

void test_enum() {
	printf("%s\n", GET_VARIANT_BODY(var, One));
}

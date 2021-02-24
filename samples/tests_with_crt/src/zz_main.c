#include <stdio.h>
#include <stdlib.h>

#define RT_DEFINE_USE_CRT

#include <rpr.h>

int main()
{
	char buffer[20];
	int i;

	for (i = 0; i < 20; i++) {
		buffer[i] = 0;
	}

	rt_memory_set(buffer, 'a', 10);
	puts(buffer);

	printf("Hello world!\n");
	return 0;
}

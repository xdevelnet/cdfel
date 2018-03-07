#include "cdfel.c"
#include <unistd.h>
#include <fcntl.h>
#include <iso646.h>
#include <stdlib.h>
#include <stdint.h>

int writer_cb(void *buffer, unsigned long amount, void *aaa) {
	if (amount <= 0 or write(STDOUT_FILENO, buffer, (size_t) amount) <= 0) return CDFEL_FAIL;
	return CDFEL_OK;
}

const char example_string[] = "What is love? Baby don't hurt me... don't hurt me... or i will punch u right in a face!\n";

void print_apples(int red, int green) {
	CDFEL_INIT(qwe, writer_cb);

	CDFEL_RANGE(qwe, example_string, sizeof(example_string) - 1);
	CDFEL_CSTRING(qwe, "\n\nAmount of red apples: ");
	CDFEL_UNSIGNED(qwe, red);
	CDFEL_CSTRING(qwe, "\n");
	CDFEL_CSTRING(qwe, "Amount of green apples: ");
	CDFEL_UNSIGNED(qwe, green);
	CDFEL_CSTRING(qwe, "\n");
	CDFEL_CSTRING(qwe, "Ok. Now here you have unsigned ptr max value: ");
	CDFEL_UNSIGNED(qwe, UINTPTR_MAX);
	CDFEL_CSTRING(qwe, ", and this is signed: ");
	CDFEL_SIGNED(qwe, INTPTR_MIN);
	CDFEL_CSTRING(qwe, ".\n");

	CDFEL_DONE(qwe);
}

int main() {
	print_apples(16, 55);
	return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	// Check arguments
	if (argc < 2) {
		fputs("Please supply at least one parameter!\n", stderr);
		return EXIT_FAILURE;
	}
	
	// Iterate over arguments
	for (size_t arg_num = 1; arg_num < argc; ++arg_num) {
		puts(argv[arg_num]);
	}
	
	return EXIT_SUCCESS;
}

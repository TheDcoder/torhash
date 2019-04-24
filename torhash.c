#include <plibsys.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void clean_exit(bool error);

int main(int argc, char *argv[]) {
	// Check arguments
	if (argc < 2) {
		fputs("Please supply at least one parameter!\n", stderr);
		return EXIT_FAILURE;
	}
	
	// Initialize plibsys
	p_libsys_init();
	
	// Initialize cryptographic hash context
	PCryptoHash *hash = p_crypto_hash_new(P_CRYPTO_HASH_TYPE_SHA1);
	if (!hash) {
		fputs("Failed to initialize cryptographic hash context!\n", stderr);
		clean_exit(false);
	}
	bool hash_error = false;
	
	// Iterate over arguments
	for (size_t arg_num = 1; arg_num < argc; ++arg_num) {
		// Add data for hashing
		p_crypto_hash_update(hash, (puchar *) argv[arg_num], strlen(argv[arg_num]));
		
		// Get the hash as a string
		char *hash_string = p_crypto_hash_get_string(hash);
		if (!hash_string) {
			fputs("Failed to compute hash!\n", stderr);
			hash_error = true;
			break;
		}
		
		// Print the hash
		puts(hash_string);
		
		// Free the string
		p_free(hash_string);
		
		// Reset the hash context
		p_crypto_hash_reset(hash);
	}
	
	// Perform a clean exit
	p_crypto_hash_free(hash);
	clean_exit(!hash_error);
}

void clean_exit(bool no_error) {
	// Clean up and exit
	p_libsys_shutdown();
	exit(no_error ? EXIT_SUCCESS : EXIT_FAILURE);
}

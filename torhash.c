#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tomcrypt.h>

// SHA1 digest length
#define DIGEST_LEN 20

// Length of RFC2440-style S2K specifier: the first 8 bytes are a salt, the
// 9th describes how much iteration to do.
#define S2K_RFC2440_SPECIFIER_LEN 9

bool secret_to_key_rfc2440(char *key_out, size_t key_out_len, const char *secret, size_t secret_len, const char *s2k_specifier);
void clean_exit();
char *bin2hex(const unsigned char *bin, size_t len);

int main(int argc, char *argv[]) {
	// Check arguments
	if (argc < 2) {
		fputs("Please supply at least one parameter!\n", stderr);
		return EXIT_FAILURE;
	}
	
	// Allocate buffer for storing the key
	char key[S2K_RFC2440_SPECIFIER_LEN + DIGEST_LEN];
	
	// Iterate over arguments
	for (size_t arg_num = 1; arg_num < argc; ++arg_num) {
		// Populate the specifier with random information
		sprng_read((unsigned char *) key, S2K_RFC2440_SPECIFIER_LEN - 1, NULL);
		key[S2K_RFC2440_SPECIFIER_LEN - 1] = 96; // This is hard-coded in tor
		
		// Hash the string
		bool success = secret_to_key_rfc2440(
			key + S2K_RFC2440_SPECIFIER_LEN,
			DIGEST_LEN,
			argv[arg_num],
			strlen(argv[arg_num]),
			key
		);
		if (!success) {
			fputs("Failed to hash the string!\n", stderr);
			continue;
		}
		
		// Convert the hash to a hexadecimal string
		char *hex_string = bin2hex((unsigned char *) key, S2K_RFC2440_SPECIFIER_LEN + DIGEST_LEN);
		if (!hex_string) {
			fputs("Failed to convert hash to hexadecimal string!\n", stderr);
			continue;
		}
		
		// Print the hex encoded key
		printf("16:%s\n", hex_string);
		free(hex_string);
	}
	
	// Perform a clean exit
	clean_exit();
}

bool secret_to_key_rfc2440(char *key_out, size_t key_out_len, const char *secret, size_t secret_len, const char *s2k_specifier) {
	char iteration_count = s2k_specifier[S2K_RFC2440_SPECIFIER_LEN - 1];
	#define EXPBIAS 6
	size_t count = ((uint32_t)16 + (iteration_count & 15)) << ((iteration_count >> 4) + EXPBIAS);
	#undef EXPBIAS
	
	// Allocate and populate the temporary buffer with data
	// This is the data which will be hashed
	char *temp = malloc((S2K_RFC2440_SPECIFIER_LEN - 1) + secret_len);
	memcpy(temp, s2k_specifier, S2K_RFC2440_SPECIFIER_LEN - 1);
	memcpy(temp + (S2K_RFC2440_SPECIFIER_LEN - 1), secret, secret_len);
	secret_len += S2K_RFC2440_SPECIFIER_LEN - 1;
	
	// Hash the data
	hash_state hash;
	if (sha1_init(&hash) != CRYPT_OK) {
		fputs("Failed to initialize cryptographic hash state!\n", stderr);
		return false;
	}
	
	int result;
	while (count != 0) {
		if (count >= secret_len) {
			result = sha1_process(&hash, (unsigned char *) temp, secret_len);
			count -= secret_len;
		} else {
			result = sha1_process(&hash, (unsigned char *) temp, count);
			count = 0;
		}
		if (result != CRYPT_OK) {
			fputs("Failed to hash data!\n", stderr);
			return false;
		}
	}
	free(temp);
	
	// Get the raw digest
	unsigned char digest[DIGEST_LEN];
	result = sha1_done(&hash, digest);
	if (result != CRYPT_OK) {
		fputs("Failed to finalize hashed data!\n", stderr);
		return false;
	}
	
	// Copy the digest
	if (key_out_len <= DIGEST_LEN) {
		memcpy(key_out, digest, key_out_len);
		return true;
	} else {
		// Key expansion is unsupported at the moment
		return false;
	}
}

void clean_exit() {
	// Clean up and exit
	exit(EXIT_SUCCESS);
}

// Forked from: https://nachtimwald.com/2017/09/24/hex-encode-and-decode-in-c/
char *bin2hex(const unsigned char *bin, size_t len) {
	if (bin == NULL || len == 0) return NULL;
	
	char *out = malloc(len*2+1);
	if (!out) return NULL;
	
	for (size_t i=0; i<len; i++) {
		out[i*2]   = "0123456789ABCDEF"[bin[i] >> 4];
		out[i*2+1] = "0123456789ABCDEF"[bin[i] & 0x0F];
	}
	out[len*2] = '\0';
	
	return out;
}

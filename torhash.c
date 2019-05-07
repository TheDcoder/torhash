#include <openssl/evp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	
	// Seed the random number generator
	srand(1337);
	
	// Allocate buffer for storing the key
	char key[S2K_RFC2440_SPECIFIER_LEN + DIGEST_LEN];
	
	// Iterate over arguments
	for (size_t arg_num = 1; arg_num < argc; ++arg_num) {
		// Populate the specifier with random information
		// NOTE: This is not cryptographically secure and works with a fixed seed
		for (size_t n = 0; n < S2K_RFC2440_SPECIFIER_LEN - 1; ++n) key[n] = (char) rand();
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
	bool success;
	EVP_MD_CTX *hash_context = EVP_MD_CTX_new();
	const EVP_MD *sha1_md = EVP_sha1();
	success = EVP_DigestInit(hash_context, sha1_md);
	if (!success) {
		fputs("Failed to initialize cryptographic hash context!\n", stderr);
		EVP_MD_CTX_free(hash_context);
		return false;
	}
	
	while (count != 0) {
		if (count >= secret_len) {
			success = EVP_DigestUpdate(hash_context, temp, secret_len);
			count -= secret_len;
		} else {
			success = EVP_DigestUpdate(hash_context, temp, count);
			count = 0;
		}
		if (!success) {
			fputs("Failed to hash data!\n", stderr);
			EVP_MD_CTX_free(hash_context);
			return false;
		}
	}
	free(temp);
	
	// Get the raw digest
	unsigned char digest[DIGEST_LEN];
	success = EVP_DigestFinal(hash_context, digest, NULL);
	EVP_MD_CTX_free(hash_context);
	if (!success) {
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

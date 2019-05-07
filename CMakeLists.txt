cmake_minimum_required (VERSION 2.6)
project (torhash C)

# Enable warnings
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
	add_compile_options(-Wall -Wpedantic)
endif()

# Main executable
add_executable(torhash torhash.c)

# Link OpenSSL libcrypto
if(UNIX)
	target_link_libraries(torhash pthread) # Required by crypto
	target_link_libraries(torhash dl) # Required by crypto
	target_link_libraries(torhash z) # Required by crypto
endif()
target_link_libraries(torhash libcrypto.a)

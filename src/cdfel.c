/*
 * Copyright (c) 2018, Xdevelnet (xdevelnet at xdevelnet dot org)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "cdfel.h"

#if defined NO_C_STDLIB // yea, and you probably haven't size_t :(
	static void *memcpy(void *dest, const void *src, unsigned long n) {
		for (unsigned long i = 0; i < n; i++) ((char *) dest)[i] = ((const char *) src)[i];
		return dest;
	}
	#if !defined NULL
		#define NULL ((void *) 0)
	#endif
#else
	#include <string.h>
	#include <iso646.h>
	#include <stddef.h>
	#include <stdint.h>
#endif

#if defined UINTPTR_MAX
	#if UINTPTR_MAX == UINT64_MAX
		#define HUGEST_NUMBER_STRING_LENGTH 21 // echo 18446744073709551615 | wc -c
	#endif
	#if UINTPTR_MAX == UINT32_MAX
		#define HUGEST_NUMBER_STRING_LENGTH 11 // echo 4294967295 | wc -c
	#endif
	#if UINTPTR_MAX == UINT16_MAX
		#define HUGEST_NUMBER_STRING_LENGTH 6 // echo 65536 | wc -c
	#endif
	#if UINTPTR_MAX == UINT8_MAX
		#define HUGEST_NUMBER_STRING_LENGTH 4 // echo 256 | wc -c
	#endif
#else
	#define HUGEST_NUMBER_STRING_LENGTH 21 // haven't stdint.h in your system? well, screw you then.
#endif


struct cdfel {
	void *buffer;
	unsigned long bsize;
	unsigned long bhead;
	int (*cb) (void *, unsigned long, void *);
	void *user_ptr;
};

const unsigned int cdfel_structure_size = sizeof(struct cdfel);

static inline int cdfel_put_mem_area_inlined(cdfel *dest, const void *vbuffer, unsigned long range) {
	if (!range) return CDFEL_OK;
	unsigned int readhead = 0;
	while(readhead != range) {
		if (range - readhead <= dest->bsize - dest->bhead) {
			unsigned long remaining = range - readhead;
			memcpy( (char *) dest->buffer + dest->bhead, (const char *) vbuffer + readhead, remaining);
			dest->bhead += remaining;
			readhead += remaining;
		} else {
			unsigned long remaining = dest->bsize - dest->bhead;
			memcpy( (char *) dest->buffer + dest->bhead, (const char *) vbuffer + readhead, remaining);
			readhead += remaining;
			dest->bhead = 0;
			if (dest->cb(dest->buffer, dest->bsize, dest->user_ptr) == CDFEL_FAIL) return CDFEL_FAIL;
		}
	}
	if (dest->bhead == dest->bsize) {
		dest->bhead = 0;
		if (dest->cb(dest->buffer, dest->bsize, dest->user_ptr) == CDFEL_FAIL) return CDFEL_FAIL;
	}
	return CDFEL_OK;
}

int cdfel_mem_range(cdfel *dest, const void *vbuffer, unsigned long range) {
	return cdfel_put_mem_area_inlined(dest, vbuffer, range);
}

void cdfel_assign(cdfel *dest, int (*cb) (void *, unsigned long, void *), void *buffer, unsigned long buffersize, void *your_ptr) {
	dest->buffer = buffer, dest->bsize = buffersize, dest->bhead = 0, dest->cb = cb, dest->user_ptr = your_ptr;
}

int cdfel_c_string(cdfel *dest, const char *str) { // I was high as fuck when I was writing this
	unsigned long i = 0; // but next day I just realized that I couldn't write better. Someone, help me - send money plz.
	const unsigned long search_limit = 10; // Still looking for good advices about this constant.
	while(str[i] != '\0') if (i == search_limit) {
		if (cdfel_mem_range(dest, str, i) == CDFEL_FAIL) return CDFEL_FAIL; 
		str += i;
		i = 0;
	} else i++;

	if (cdfel_mem_range(dest, str, i) == CDFEL_FAIL) return CDFEL_FAIL;

	return CDFEL_OK;
}

static inline char __cdfel_inline_unsigned_int_to_str(char *str, unsigned long long value) {
	unsigned char ii = 0;
	unsigned long long int divider = value;
	// thanks to my dear brotha for this procedure. Love him <3
	do {
		ii++;
		divider /= 10;
	} while (divider != 0);

	divider = 10;

	unsigned char len = ii;
	str[ii] = '\0';

	while (ii > 0) {
		str[--ii] = (value % divider) / (divider / 10) + '0';
		divider *= 10;
	}
	
	return len;
}


int cdfel_unsigned(cdfel *dest, unsigned long long value) {
	char result[HUGEST_NUMBER_STRING_LENGTH];
	if (cdfel_mem_range(dest, result, __cdfel_inline_unsigned_int_to_str(result, value)) == CDFEL_FAIL) return CDFEL_FAIL;
	return CDFEL_OK;
}

int cdfel_signed(cdfel *dest, union __cdfel_private_int_union value) { // yea, bitchez, that's what I call "simple beauty"
	char result[HUGEST_NUMBER_STRING_LENGTH];
	unsigned char result_len = 0;
	if (value.sig < 0) {
		result[result_len++] = '-';
		value.un = (unsigned long long int) -value.sig;
	} else value.un = (unsigned long long int) value.sig;
	result_len += __cdfel_inline_unsigned_int_to_str(result + result_len, value.un);
	if (cdfel_mem_range(dest, result, result_len) == CDFEL_FAIL) return CDFEL_FAIL;
	return CDFEL_OK;
}

void cdfel_done(cdfel *dest) {
	if (dest->bhead != 0) dest->cb(dest->buffer, dest->bhead, dest->user_ptr), dest->bhead = 0;
	//dest->cb(dest->buffer, 0, dest->user_ptr);
}

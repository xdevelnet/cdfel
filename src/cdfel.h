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

#define CDFEL_FAIL -1
#define CDFEL_OK 0
#define CDFEL_DEFAULT_BUFFER_SIZE 20

typedef struct cdfel cdfel;
extern const unsigned int cdfel_structure_size;

void cdfel_assign(cdfel *instance, int (*cb) (void *, unsigned long, void *), void *buffer, unsigned long buffersize, void *your_ptr);
int cdfel_mem_range(cdfel *dest, const void *vbuffer, unsigned long range);
int cdfel_c_string(cdfel *dest, const char *str);
void cdfel_done(cdfel *dest);
int cdfel_unsigned(cdfel *dest, unsigned long long value);

union __cdfel_private_int_union {
	unsigned long long int un;
	signed long long int sig;
};

int cdfel_signed(cdfel *dest, union __cdfel_private_int_union value);

#define CDFEL_CERT_JOIN(x, y) CDFEL_CERT_JOIN_AGAIN(x, y)
#define CDFEL_CERT_JOIN_AGAIN(x, y) x ## y

#define CDFEL_INIT_FULL(inst, cb, buffer, buffersize, your_ptr)\
	cdfel_assign((cdfel *) (inst), cb, buffer, buffersize, your_ptr);

#define CDFEL_INIT_YB(inst, cb, buffer, buffersize)\
	CDFEL_INIT_FULL(inst, cb, buffer, buffersize, ((void *) 0));

#define CDFEL_INIT_B(inst, cb, buffersize)\
	char CDFEL_CERT_JOIN(inst, _cdfel_private_buffer) [buffersize];\
	char CDFEL_CERT_JOIN(inst, _cdfel_struct) [cdfel_structure_size];\
	CDFEL_INIT_YB(CDFEL_CERT_JOIN(inst, _cdfel_struct), cb, CDFEL_CERT_JOIN(inst, _cdfel_private_buffer), buffersize)

#define CDFEL_INIT(inst, cb)\
	char CDFEL_CERT_JOIN(inst, _cdfel_private_buffer) [CDFEL_DEFAULT_BUFFER_SIZE];\
	char CDFEL_CERT_JOIN(inst, _cdfel_struct) [cdfel_structure_size];\
	CDFEL_INIT_YB(CDFEL_CERT_JOIN(inst, _cdfel_struct), cb, CDFEL_CERT_JOIN(inst, _cdfel_private_buffer), CDFEL_DEFAULT_BUFFER_SIZE)

#define CDFEL_RANGE(inst, ptr, size)\
	if (cdfel_mem_range((cdfel *) CDFEL_CERT_JOIN(inst, _cdfel_struct), ptr, size) == CDFEL_FAIL) goto CDFEL_CERT_JOIN(inst, _cdfel_label);

#define CDFEL_CSTRING(inst, ptr)\
	if (cdfel_c_string((cdfel *) CDFEL_CERT_JOIN(inst, _cdfel_struct), ptr) == CDFEL_FAIL) goto CDFEL_CERT_JOIN(inst, _cdfel_label);

#define CDFEL_DONE(inst)\
	CDFEL_CERT_JOIN(inst, _cdfel_label) :\
	cdfel_done((cdfel *) CDFEL_CERT_JOIN(inst, _cdfel_struct));

#define CDFEL_UNSIGNED(inst, val)\
	if (cdfel_unsigned((cdfel *) CDFEL_CERT_JOIN(inst, _cdfel_struct), val) == CDFEL_FAIL) goto CDFEL_CERT_JOIN(inst, _cdfel_label);

#define CDFEL_SIGNED(inst, val)\
	if (cdfel_signed((cdfel *) CDFEL_CERT_JOIN(inst, _cdfel_struct), (union __cdfel_private_int_union) (signed long long int) val) == CDFEL_FAIL) goto CDFEL_CERT_JOIN(inst, _cdfel_label);

// uuu blya, makrosniy tupnyak, vse kak vi lybite. uda4nogo debagga pacani

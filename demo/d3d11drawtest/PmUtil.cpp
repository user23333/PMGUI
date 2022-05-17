/*
Author: CSlime
Github: https://github.com/cs1ime
*/

#include "PmUtil.h"

void pm_zeromem(void *addr, int sz) {
    while (sz)
        ((unsigned char *)addr)[sz-- - 1] = 0;
}
void pm_memcpy(void* dst, const void* src, int sz) {
    while (sz)
        ((unsigned char*)dst)[sz - 1] = ((unsigned char*)src)[sz-- - 1];
}
void pm_strcpy(char* dst, const char* src) {
    while (*dst++ = *src++);
}
void pm_xor_data(void* data, int size, unsigned int xor_key) {
	char key[4];
	*(unsigned int*)&key = xor_key ^ 0xFF998811;
	for (int i = 0; i < size; i++) {
		((unsigned char*)data)[i] ^= key[i % 3];
	}
}

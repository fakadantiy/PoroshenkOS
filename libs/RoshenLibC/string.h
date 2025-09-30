//#pragma once
// doesn't work for some reason
#ifndef LIBC_STRING
#define LIBC_STRING

static inline void * memcpy(void * s1, const void * s2, unsigned long n) {
	char * dest = ( char * ) s1;
	const char * src = ( const char * ) s2;

	while (n--) {
	/*
		if ( dest == s2 || src == s1 ) {
			//goto runtime_constraint_violation;
			asm ("int $0x0D"); // error 13
	*/
		*dest++ = *src++;
	}
	return dest;
}

static inline void * memset(void * s, int c, unsigned long n){
	unsigned char * p = ( unsigned char * ) s;
	while (n--) {
		*p++ = ( unsigned char ) c;
	}
	return s;
}

static inline int strcmp(const char* s1, const char* s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(unsigned char*)s1 - *(unsigned char*)s2;;
}
#endif

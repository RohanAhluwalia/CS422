#ifndef _KERN_STRING_H_
#define _KERN_STRING_H_

#ifdef _KERN_

#include "types.h"

int     strnlen(const char *s, size_t size);

void *	memset(void *dst, int c, size_t len);
void *	memzero(void *dst, size_t len);
void *	memcpy(void *dst, const void *src, size_t len);
void *	memmove(void *dst, const void *src, size_t len);

#endif /* _KERN_ */

#endif /* !_KERN_STRING_H_ */
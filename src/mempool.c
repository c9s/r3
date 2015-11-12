#include <stdio.h>
#include <stdlib.h>
#include "mempool.h"

void ex_mpool_init(ex_mpool *pmp, char *begin, size_t len)
{
	pmp->begin = begin;
	pmp->len = len;
	pmp->index = 0;
	pmp->cflag = 0;
}

void *ex_mpool_malloc(ex_mpool *pmp, size_t mlen)
{
	void *ret = NULL;

	size_t rIndex = pmp->index + mlen;
	if (rIndex > pmp->len) {
		ret = malloc(mlen);
		pmp->cflag = 1;
	}
	else {
		ret = pmp->begin + pmp->index;
		pmp->index = rIndex;
	}
	return ret;
}

void ex_mpool_free(ex_mpool *pmp, void *p)
{
	/* only perform free when allocated in heap */
	if (p < (void *) pmp->begin ||
	    p >= (void *) (pmp->begin + pmp->len)) {
		free(p);
	}
}

void ex_mpool_clear(ex_mpool *pmp)
{
	pmp->index = 0;
	pmp->cflag = 0;
}


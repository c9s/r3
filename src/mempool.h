#ifndef __ESERV_MPOOL_H__
#define __ESERV_MPOOL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	char *begin;	/* start pos */
	size_t len;	/* capacity */
	int index;	/* curIndex */
	int cflag;	/* clear flag */
} ex_mpool;

void ex_mpool_init(ex_mpool *pmp, char *begin, size_t len);
void *ex_mpool_malloc(ex_mpool *pmp, size_t mlen);
void ex_mpool_free(ex_mpool *pmp, void *p);
void ex_mpool_clear(ex_mpool *pmp);

#ifdef __cplusplus
}
#endif
#endif


#ifndef R3_PERSISTENT_H
#define R3_PERSISTENT_H 1

extern inline int persistent_store(char *key, int key_len, int list_type, void * val TSRMLS_DC);
extern inline int r3_persistent_store(char *ns, char *key, int list_type, void * val TSRMLS_DC) ;

extern inline void * persistent_fetch(char *key, int key_len TSRMLS_DC);
extern inline void * r3_persistent_fetch(char *ns, char *key TSRMLS_DC);

#endif

#ifndef PHP_R3_H
#define PHP_R3_H 1

#include "php.h"
#include "string.h"
#include "main/php_main.h"
#include "Zend/zend_API.h"
#include "Zend/zend_variables.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "zend_object_handlers.h"
#include "ext/standard/php_string.h"

#define PHP_R3_VERSION "1.3.1"
#define PHP_R3_EXTNAME "r3"


#ifdef ZTS
#include "TSRM.h"
#endif

extern int r3_globals_id;

extern int le_mux_hash_table;

// global variable structure
ZEND_BEGIN_MODULE_GLOBALS(r3)
    // zval *mux_array;
    HashTable * persistent_list;
    // zend_bool direction;
ZEND_END_MODULE_GLOBALS(r3)

#ifdef ZTS
#define R3_G(v) TSRMG(r3_globals_id, zend_r3_globals *, v)
#else
#define R3_G(v) (r3_globals.v)
#endif



#define ZEND_HASH_FETCH(hash,key,ret) \
    zend_hash_find(hash, key, sizeof(key), (void**)&ret) == SUCCESS

#define PUSH_PARAM(arg) zend_vm_stack_push(arg TSRMLS_CC)
#define POP_PARAM() (void)zend_vm_stack_pop(TSRMLS_C)
#define PUSH_EO_PARAM()
#define POP_EO_PARAM()
 
#define CALL_METHOD_BASE(classname, name) zim_##classname##_##name
 
#define CALL_METHOD_HELPER(classname, name, retval, thisptr, num, param) \
  PUSH_PARAM(param); PUSH_PARAM((void*)num);                            \
  PUSH_EO_PARAM();                                                      \
  CALL_METHOD_BASE(classname, name)(num, retval, NULL, thisptr, 0 TSRMLS_CC); \
  POP_EO_PARAM();                       \
  POP_PARAM(); POP_PARAM();
 
#define CALL_METHOD(classname, name, retval, thisptr)                  \
  CALL_METHOD_BASE(classname, name)(0, retval, NULL, thisptr, 0 TSRMLS_CC);
 
#define CALL_METHOD1(classname, name, retval, thisptr, param1)         \
  CALL_METHOD_HELPER(classname, name, retval, thisptr, 1, param1);
 
#define CALL_METHOD2(classname, name, retval, thisptr, param1, param2) \
  PUSH_PARAM(param1);                                                   \
  CALL_METHOD_HELPER(classname, name, retval, thisptr, 2, param2);     \
  POP_PARAM();
 
#define CALL_METHOD3(classname, name, retval, thisptr, param1, param2, param3) \
  PUSH_PARAM(param1); PUSH_PARAM(param2);                               \
  CALL_METHOD_HELPER(classname, name, retval, thisptr, 3, param3);     \
  POP_PARAM(); POP_PARAM();

PHP_MINIT_FUNCTION(r3);
PHP_MSHUTDOWN_FUNCTION(r3);
PHP_RINIT_FUNCTION(r3);

/*
zval * php_r3_match(zval *z_routes, char *path, int path_len TSRMLS_DC);

zval * call_mux_method(zval * object , char * method_name , int method_name_len, int param_count, zval* arg1, zval* arg2, zval* arg3 TSRMLS_DC);

zend_class_entry ** get_pattern_compiler_ce(TSRMLS_D);

extern zend_class_entry *ce_r3_exception;
*/

extern zend_module_entry r3_module_entry;

void r3_init_exception(TSRMLS_D);

void r3_mux_le_hash_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC);

PHP_FUNCTION(r3_match);

#define phpext_r3_ptr &r3_module_entry

#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "string.h"
#include "main/php_main.h"
#include "Zend/zend_API.h"
#include "Zend/zend_variables.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "zend_object_handlers.h"
#include "ext/standard/php_string.h"

#include "php_r3.h"
// #include "ct_helper.h"
// #include "r3_functions.h"
// #include "r3_mux.h"
// #include "php_expandable_mux.h"
// #include "r3_controller.h"

ZEND_DECLARE_MODULE_GLOBALS(r3);


// persistent list entry type for HashTable
int le_mux_hash_table;

// persistent list entry type for boolean
int le_mux_bool;

// persistent list entry type for int
int le_mux_int;

// persistent list entry type for string
int le_mux_string;

zend_class_entry *ce_r3_exception;


// #define DEBUG 1
static const zend_function_entry r3_functions[] = {
    PHP_FE(r3_match, NULL)
    PHP_FE_END
};

void r3_init_exception(TSRMLS_D) {
  zend_class_entry e;
  INIT_CLASS_ENTRY(e, "R3Exception", NULL);
  ce_r3_exception = zend_register_internal_class_ex(&e, (zend_class_entry*)zend_exception_get_default(TSRMLS_C), NULL TSRMLS_CC);
}

void r3_mux_le_hash_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    HashTable *h = (HashTable*) rsrc->ptr;
    if (h) {
        // zend_hash_destroy(h);
        // pefree(h, 1);
    }
}

zend_module_entry r3_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_R3_EXTNAME,
    r3_functions,
    PHP_MINIT(r3),
    PHP_MSHUTDOWN(r3),
    PHP_RINIT(r3),
    NULL,
    NULL,
    PHP_R3_VERSION,
    STANDARD_MODULE_PROPERTIES
};

PHP_INI_BEGIN()
    PHP_INI_ENTRY("r3.fstat", "0", PHP_INI_ALL, NULL)
    // STD_PHP_INI_ENTRY("r3.direction", "1", PHP_INI_ALL, OnUpdateBool, direction, zend_hello_globals, hello_globals)
PHP_INI_END()

#ifdef COMPILE_DL_R3
ZEND_GET_MODULE(r3)
#endif

static void php_r3_init_globals(zend_r3_globals *r3_globals)
{
    // r3_globals->persistent_list = (HashTable*) 
    // array_init(r3_globals->persistent_list);
}

PHP_MINIT_FUNCTION(r3) {
  ZEND_INIT_MODULE_GLOBALS(r3, php_r3_init_globals, NULL);
  REGISTER_INI_ENTRIES();
  // r3_init_mux(TSRMLS_C);
  // r3_init_expandable_mux(TSRMLS_C);
  // r3_init_controller(TSRMLS_C);
  le_mux_hash_table = zend_register_list_destructors_ex(NULL, r3_mux_le_hash_dtor, "hash table", module_number);
  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(r3) {
  UNREGISTER_INI_ENTRIES();
  return SUCCESS;
}

PHP_RINIT_FUNCTION(r3) {
  return SUCCESS;
}


/*
 * r3_compile(array $routes, string $path);
 */
PHP_FUNCTION(r3_match)
{
    zval *z_routes;
    char *path;
    int  path_len;

    /* parse parameters */
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "as", 
                    &z_routes, 
                    &path, &path_len ) == FAILURE) {
        RETURN_FALSE;
    }

    /*
    zval *z_route;
    z_route = php_r3_match(z_routes, path, path_len TSRMLS_CC);
    if ( z_route != NULL ) {
        *return_value = *z_route;
        zval_copy_ctor(return_value);
        return;
    }
    */
    RETURN_NULL();
}


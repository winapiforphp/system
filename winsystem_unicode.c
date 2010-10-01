/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Elizabeth Smith <auroraeosrose@php.net>                      |
  +----------------------------------------------------------------------+
*/

#include "php_winsystem.h"
#include "zend_exceptions.h"

ZEND_DECLARE_MODULE_GLOBALS(winsystem);

PHP_WINSYSTEM_API zend_class_entry *ce_winsystem_unicode;
static zend_object_handlers winsystem_unicode_object_handlers;
static zend_function winsystem_unicode_constructor_wrapper;

WCHAR * win_system_convert_to_wchar(const CHAR ** utf8_string, int type);
CHAR * win_system_convert_to_char(const WCHAR ** utf16_string, int type);

/* ----------------------------------------------------------------
   Win\System\Unicode Userland API                                                      
------------------------------------------------------------------*/
ZEND_BEGIN_ARG_INFO_EX(WinSystemUnicode___construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, string)
	ZEND_ARG_INFO(0, type)
ZEND_END_ARG_INFO()

/* {{{ proto array Win\System\Unicode->__construct()
     Creates a new Win system unicode object */
PHP_METHOD(WinSystemUnicode, __construct)
{
	char * name = NULL;
	int name_length;
	long type = 0;
	zend_error_handling error_handling;
	winsystem_unicode_object *unicode_object = (winsystem_unicode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &name, &name_length, &type) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (type == 0) {
		/* we're already utf-16, copy it into the wchar_t */
		unicode_object->unicode_string = emalloc(name_length * sizeof(wchar_t));
		memcpy(unicode_object->unicode_string, name, name_length * sizeof(char));
		unicode_object->multibyte_string = win_system_convert_to_char(&unicode_object->unicode_string, type);
	} else {
		unicode_object->unicode_string = win_system_convert_to_wchar(&name, type);
		unicode_object->multibyte_string = estrdup(name);
	}

	unicode_object->is_constructed = TRUE;
}
/* }}} */
 
static zend_function *get_constructor(zval *object TSRMLS_DC)
{
    /* Could always return constr_wrapper_fun, but it's uncessary to call the
     * wrapper if instantiating the superclass */
    if (Z_OBJCE_P(object) == ce_winsystem_unicode)
        return zend_get_std_object_handlers()->
            get_constructor(object TSRMLS_CC);
    else
        return &winsystem_unicode_constructor_wrapper;
}
 
static void construction_wrapper(INTERNAL_FUNCTION_PARAMETERS) {
    zval *this = getThis();
    winsystem_unicode_object *tobj;
    zend_class_entry *this_ce;
    zend_function *zf;
    zend_fcall_info fci = {0};
    zend_fcall_info_cache fci_cache = {0};
    zval *retval_ptr = NULL;
    unsigned i;
 
    tobj = zend_object_store_get_object(this TSRMLS_CC);
    zf = zend_get_std_object_handlers()->get_constructor(this TSRMLS_CC);
    this_ce = Z_OBJCE_P(this);
 
    fci.size = sizeof(fci);
    fci.function_table = &this_ce->function_table;
    fci.object_ptr = this;
    /* fci.function_name = ; not necessary to bother */
    fci.retval_ptr_ptr = &retval_ptr;
    fci.param_count = ZEND_NUM_ARGS();
    fci.params = emalloc(fci.param_count * sizeof *fci.params);
    /* Or use _zend_get_parameters_array_ex instead of loop: */
    for (i = 0; i < fci.param_count; i++) {
        fci.params[i] = (zval **) (zend_vm_stack_top(TSRMLS_C) - 1 -
            (fci.param_count - i));
    }
    fci.object_ptr = this;
    fci.no_separation = 0;
 
    fci_cache.initialized = 1;
    fci_cache.called_scope = EG(current_execute_data)->called_scope;
    fci_cache.calling_scope = EG(current_execute_data)->current_scope;
    fci_cache.function_handler = zf;
    fci_cache.object_ptr = this;
 
    zend_call_function(&fci, &fci_cache TSRMLS_CC);
    if (!EG(exception) && tobj->is_constructed == 0)
		zend_throw_exception_ex(NULL, 0 TSRMLS_CC,
			"parent::__construct() must be called in %s::__construct()", this_ce->name);
    efree(fci.params);
    zval_ptr_dtor(&retval_ptr);
}

/* register unicode methods */
static zend_function_entry winsystem_unicode_functions[] = {
	PHP_ME(WinSystemUnicode, __construct, WinSystemUnicode___construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	{NULL, NULL, NULL}
};

/* ----------------------------------------------------------------
  C API for unicode functions                                                
------------------------------------------------------------------*/

/* {{{ win_system_convert_to_wchar
       api converts char * of charset type to utf16/wchar */
WCHAR * win_system_convert_to_wchar(const CHAR ** utf8_string, int type)
{
	wchar_t * utf16_string = NULL;
	int utf16_string_length, result;

	switch (type) {
	}

	utf16_string_length = MultiByteToWideChar(
		type,         /* convert from UTF-8 */
        0,            /* error on invalid chars */
        *utf8_string, /* source UTF-8 string */
        -1,           /* total length of source UTF-8 string, in CHAR's (= bytes), including end-of-string \0 */
        NULL,         /* unused - no conversion done in this step */
        0             /* request size of destination buffer, in WCHAR's */
   );

	// TODO: utf8_string_length and utf16_string_length cannot be > INT_MAX - 1 or we chop chop
	
    utf16_string = emalloc(utf16_string_length * sizeof(wchar_t));

    result = MultiByteToWideChar(
        type,               /* convert from UTF-8 */
        0,                  /* error on invalid chars */
        *utf8_string,       /* source UTF-8 string */
        -1,                 /* total length of source UTF-8 string */
        utf16_string,       /* destination buffer */
        utf16_string_length /* size of destination buffer, in WCHAR's */
    );

	// TODO: error handle, result better == utf16 string length

	return utf16_string;
}
/* }}} */

/* {{{ win_system_convert_to_char
       api converts wchar to char * of charset type */
CHAR * win_system_convert_to_char(const WCHAR ** utf16_string, int type)
{
	char * utf8_string = NULL;
	int utf8_string_length, result;

	switch (type) {
	}

	utf8_string_length = WideCharToMultiByte(
		CP_UTF8,       /* convert from UTF-8 */
		0,             /* error on invalid chars*/
		*utf16_string, /* source UTF-8 string*/
		-1,            /* total length of source UTF-8 string, in CHAR's (= bytes), including end-of-string \0 */
		NULL,          /* unused - no conversion done in this step */
		0,              /* request size of destination buffer, in WCHAR's */
		NULL, NULL
	);

	// TODO: utf8_string_length and utf16_string_length cannot be > INT_MAX - 1 or we chop chop
	
	utf8_string = emalloc(utf8_string_length * sizeof(char));

    result = WideCharToMultiByte(
		CP_UTF8,                // convert from UTF-8
		0,   // error on invalid chars
		*utf16_string,            // source UTF-8 string
		-1,                 // total length of source UTF-8 string, in CHAR's (= bytes), including end-of-string \0
		utf8_string,               // destination buffer
		utf8_string_length,                // size of destination buffer, in WCHAR's
		NULL, NULL
	);

	// TODO: error handle, result better == utf16 string length

	return utf8_string;
}
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Unicode Custom Object magic                                                 
------------------------------------------------------------------*/

/* {{{ winsystem_unicode_object_free
       frees up the wchar underneath */
static void winsystem_unicode_object_free(void *object TSRMLS_DC)
{
	winsystem_unicode_object *unicode_object = (winsystem_unicode_object *)object;

	zend_object_std_dtor(&unicode_object->std TSRMLS_CC);

	if(unicode_object->unicode_string) {
		efree(unicode_object->unicode_string);
	}

	if(unicode_object->multibyte_string) {
		efree(unicode_object->multibyte_string);
	}
	
	efree(unicode_object);
}
/* }}} */

/* {{{ winsystem_unicode_object_create
       object that has an internal WCHAR stored  */
static zend_object_value winsystem_unicode_object_create(zend_class_entry *ce TSRMLS_DC)
{
    zend_object_value retval;
    winsystem_unicode_object       *unicode_object;
 
	unicode_object = ecalloc(1, sizeof(winsystem_unicode_object));
    zend_object_std_init((zend_object *) unicode_object, ce TSRMLS_CC);
	unicode_object->unicode_string = NULL;
	unicode_object->multibyte_string = NULL;
	unicode_object->is_constructed = FALSE;
 
    zend_hash_copy(unicode_object->std.properties, &(ce->default_properties),
        (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval*));
 
    retval.handle = zend_objects_store_put(unicode_object,
        (zend_objects_store_dtor_t) zend_objects_destroy_object,
        (zend_objects_free_object_storage_t) winsystem_unicode_object_free,
        NULL TSRMLS_CC);
    retval.handlers = &winsystem_unicode_object_handlers;
    return retval;
}
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Unicode Object Magic LifeCycle Functions                                                    
------------------------------------------------------------------ */
PHP_MINIT_FUNCTION(winsystem_unicode)
{
    zend_class_entry ce;
 
    memcpy(&winsystem_unicode_object_handlers, zend_get_std_object_handlers(),
        sizeof winsystem_unicode_object_handlers);
    winsystem_unicode_object_handlers.get_constructor = get_constructor;
 
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Unicode", winsystem_unicode_functions);

    ce_winsystem_unicode = zend_register_internal_class(&ce TSRMLS_CC);
    ce_winsystem_unicode->create_object = winsystem_unicode_object_create;
 
    winsystem_unicode_constructor_wrapper.type = ZEND_INTERNAL_FUNCTION;
    winsystem_unicode_constructor_wrapper.common.function_name = "internal_construction_wrapper";
    winsystem_unicode_constructor_wrapper.common.scope = ce_winsystem_unicode;
    winsystem_unicode_constructor_wrapper.common.fn_flags = ZEND_ACC_PROTECTED;
    winsystem_unicode_constructor_wrapper.common.prototype = NULL;
    winsystem_unicode_constructor_wrapper.common.required_num_args = 0;
    winsystem_unicode_constructor_wrapper.common.arg_info = NULL;
    winsystem_unicode_constructor_wrapper.common.pass_rest_by_reference = 0;
    winsystem_unicode_constructor_wrapper.common.return_reference = 0;
    winsystem_unicode_constructor_wrapper.internal_function.handler = construction_wrapper;
    winsystem_unicode_constructor_wrapper.internal_function.module = EG(current_module);

	zend_declare_class_constant_long(ce_winsystem_unicode, "UTF_16", sizeof("UTF_16")-1, 0 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_unicode, "UTF_8", sizeof("UTF_8")-1, CP_UTF8 TSRMLS_CC);
 
    return SUCCESS;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
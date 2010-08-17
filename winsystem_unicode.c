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

/* $Id$ */

#include "php_winsystem.h"
#include "zend_exceptions.h"

ZEND_DECLARE_MODULE_GLOBALS(winsystem);

PHP_WINSYSTEM_API zend_class_entry *ce_winsystem_unicode;
WCHAR * win_system_convert_to_wchar(const CHAR ** utf8_string, int type);
CHAR * win_system_convert_to_char(const WCHAR ** utf16_string, int type);

/* In the header because unicode is used everywhere */
extern inline winsystem_unicode_object* winsystem_unicode_object_get(zval *zobj TSRMLS_DC)
{
    winsystem_unicode_object *pobj = zend_object_store_get_object(zobj TSRMLS_CC);
    if (pobj->unicode_string == NULL) {
		php_error(E_ERROR, "Internal wchar_t missing in %s class, you must call parent::__construct in extended classes", Z_OBJCE_P(zobj)->name);
		return NULL;
    }
    return pobj;
}

/* ----------------------------------------------------------------
  Win\System\Unicode Userland API                                                    
------------------------------------------------------------------*/

/* {{{ proto bool Win\System\Unicode->__construct(binary string)
       creates a new event */
PHP_METHOD(WinSystemUnicode, __construct)
{
	char * name = NULL;
	int name_length;
	long type = 0;
	zend_error_handling error_handling;
	HWND hwnd;
	WNDCLASSEXW wcx = { 0 };
	ATOM worked;
	winsystem_unicode_object *unicode_object = (winsystem_unicode_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &name, &name_length, &type) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (type == 0) {
		// we're already utf-16, copy it into the wchar_t
		unicode_object->unicode_string = emalloc(name_length * sizeof(wchar_t));
		memcpy(unicode_object->unicode_string, name, name_length * sizeof(char));
		unicode_object->multibyte_string = win_system_convert_to_char(&unicode_object->unicode_string, type);
	} else {
		unicode_object->unicode_string = win_system_convert_to_wchar(&name, type);
		unicode_object->multibyte_string = estrdup(name);
	}


	/* these are default values that can be overridden */
	wcx.cbSize = sizeof(wcx);							
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = GetModuleHandle(NULL);							
	wcx.lpfnWndProc = DefWindowProc;					
	wcx.lpszMenuName =  NULL;							
	wcx.lpszClassName = L"php_wingui_window";					
	wcx.style = 0;
	wcx.hIcon = NULL;								
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
	wcx.hIconSm = NULL;
	
	worked = RegisterClassExW(&wcx);

	hwnd = CreateWindowW(
		L"php_wingui_window", 
        unicode_object->unicode_string,   
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
        100,  
        100,  
        500, 
        500,
        NULL,
        NULL,  
        GetModuleHandle(NULL),
        (LPVOID) NULL);
	Sleep(2000);
	DestroyWindow(hwnd);
	worked = UnregisterClass("php_wingui_window", GetModuleHandle(NULL));
	
}
/* }}} */

/* register unicode methods */
static zend_function_entry winsystem_unicode_functions[] = {
	PHP_ME(WinSystemUnicode, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	{NULL, NULL, NULL}
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Unicode Object Magic LifeCycle Functions                                                    
------------------------------------------------------------------*/
//----------------------------------------------------------------------------

// FUNCTION: ConvertUTF8ToUTF16

// DESC: Converts Unicode UTF-8 text to Unicode UTF-16 (Windows default).

//----------------------------------------------------------------------------

WCHAR * win_system_convert_to_wchar(const CHAR ** utf8_string, int type)
{
	wchar_t * utf16_string = NULL;
	int utf16_string_length, result;

	switch (type) {
	}

	utf16_string_length = MultiByteToWideChar(

        type,                // convert from UTF-8

        0,   // error on invalid chars

        *utf8_string,            // source UTF-8 string

        -1,                 // total length of source UTF-8 string,

                                // in CHAR's (= bytes), including end-of-string \0

        NULL,                   // unused - no conversion done in this step

        0                       // request size of destination buffer, in WCHAR's

        );

	// TODO: utf8_string_length and utf16_string_length cannot be > INT_MAX - 1 or we chop chop
	
	utf16_string = emalloc(utf16_string_length * sizeof(wchar_t));

    result = MultiByteToWideChar(

        type,                // convert from UTF-8

        0,   // error on invalid chars

        *utf8_string,            // source UTF-8 string

        -1,                 // total length of source UTF-8 string,

                                // in CHAR's (= bytes), including end-of-string \0

        utf16_string,               // destination buffer

        utf16_string_length                // size of destination buffer, in WCHAR's

        );

	// TODO: error handle, result better == utf16 string length

	return utf16_string;

}

CHAR * win_system_convert_to_char(const WCHAR ** utf16_string, int type)
{
	char * utf8_string = NULL;
	int utf8_string_length, result;

	switch (type) {
	}

	utf8_string_length = WideCharToMultiByte(

        CP_UTF8,                // convert from UTF-8

        0,   // error on invalid chars

        *utf16_string,            // source UTF-8 string

        -1,                 // total length of source UTF-8 string,

                                // in CHAR's (= bytes), including end-of-string \0

        NULL,                   // unused - no conversion done in this step

        0                       // request size of destination buffer, in WCHAR's

        ,NULL, NULL);



	// TODO: utf8_string_length and utf16_string_length cannot be > INT_MAX - 1 or we chop chop
	
	utf8_string = emalloc(utf8_string_length * sizeof(char));

    result = WideCharToMultiByte(

        CP_UTF8,                // convert from UTF-8

        0,   // error on invalid chars

        *utf16_string,            // source UTF-8 string

        -1,                 // total length of source UTF-8 string,

                                // in CHAR's (= bytes), including end-of-string \0

        utf8_string,               // destination buffer

        utf8_string_length,                // size of destination buffer, in WCHAR's
NULL, NULL
        );

	// TODO: error handle, result better == utf16 string length

	return utf8_string;

}

/* frees up the wchar underneath */
static void winsystem_unicode_object_destructor(void *object TSRMLS_DC)
{
	winsystem_unicode_object *unicode_object = (winsystem_unicode_object *)object;

	zend_object_std_dtor(&unicode_object->std TSRMLS_CC);

	if(unicode_object->unicode_string) {
		efree(unicode_object->unicode_string);
	}

	if(unicode_object->multibyte_string) {
		efree(unicode_object->multibyte_string);
	}
	
	efree(object);
}
/* }}} */

/* {{{ winsystem_unicode_object_new
       object that has an internal WCHAR stored */
static zend_object_value winsystem_unicode_object_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	winsystem_unicode_object *unicode_object;
	zval *tmp;

	unicode_object = ecalloc(1, sizeof(winsystem_unicode_object));
	memset(&unicode_object->std, 0, sizeof(zend_object));
	unicode_object->unicode_string = NULL;
	unicode_object->multibyte_string = NULL;

	zend_object_std_init(&unicode_object->std, ce TSRMLS_CC);
	zend_hash_copy(unicode_object->std.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(unicode_object, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) winsystem_unicode_object_destructor, NULL TSRMLS_CC);
	retval.handlers = zend_get_std_object_handlers();
	return retval;
}
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Unicode LifeCycle Functions                                                    
------------------------------------------------------------------*/
PHP_MINIT_FUNCTION(winsystem_unicode)
{
	zend_class_entry ce;
	
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Unicode", winsystem_unicode_functions);
	ce_winsystem_unicode = zend_register_internal_class(&ce TSRMLS_CC);
	ce_winsystem_unicode->create_object = winsystem_unicode_object_new;

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
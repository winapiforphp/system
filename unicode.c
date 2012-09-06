/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
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
zend_class_entry *ce_winsystem_codepage;
static zend_object_handlers winsystem_unicode_object_handlers;
static zend_function winsystem_unicode_constructor_wrapper;

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

	if (type == 1200) {
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

static void construction_wrapper(INTERNAL_FUNCTION_PARAMETERS)
{
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
PHP_WINSYSTEM_API WCHAR * win_system_convert_to_wchar(const CHAR ** utf8_string, int type)
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
PHP_WINSYSTEM_API CHAR * win_system_convert_to_char(const WCHAR ** utf16_string, int type)
{
	char * utf8_string = NULL;
	int utf8_string_length, result;

	switch (type) {
	}

	utf8_string_length = WideCharToMultiByte(
		type,          /* convert from UTF-8 */
		0,             /* error on invalid chars*/
		*utf16_string, /* source UTF-8 string*/
		-1,            /* total length of source UTF-8 string, in CHAR's (= bytes), including end-of-string \0 */
		NULL,          /* unused - no conversion done in this step */
		0,             /* request size of destination buffer, in WCHAR's */
		NULL, NULL
	);

	// TODO: utf8_string_length and utf16_string_length cannot be > INT_MAX - 1 or we chop chop
	
	utf8_string = emalloc(utf8_string_length * sizeof(char));

	result = WideCharToMultiByte(
		type,                // convert from UTF-8
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
	zend_object_value              retval;
	winsystem_unicode_object       *unicode_object;
 
	unicode_object = ecalloc(1, sizeof(winsystem_unicode_object));
	zend_object_std_init(&unicode_object->std, ce TSRMLS_CC);
	unicode_object->unicode_string = NULL;
	unicode_object->multibyte_string = NULL;
	unicode_object->is_constructed = FALSE;
 
	object_properties_init(&unicode_object->std, ce);
 
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
	zend_class_entry ce, code_ce;
 
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
	winsystem_unicode_constructor_wrapper.internal_function.handler = construction_wrapper;
	winsystem_unicode_constructor_wrapper.internal_function.module = EG(current_module);

	INIT_NS_CLASS_ENTRY(code_ce, PHP_WINSYSTEM_NS, "CodePage", NULL);
	ce_winsystem_codepage = zend_register_internal_class(&code_ce TSRMLS_CC);
	ce_winsystem_codepage->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS | ZEND_ACC_FINAL_CLASS;

	zend_declare_class_constant_long(ce_winsystem_codepage, "UTF16", sizeof("UTF16")-1, 1200 TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "ANSI",        sizeof("ANSI")-1,        CP_ACP TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "THREAD_ANSI", sizeof("THREAD_ANSI")-1, CP_THREAD_ACP TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "OEM",         sizeof("OEM")-1,         CP_OEMCP TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "MAC",         sizeof("MAC")-1,         CP_MACCP TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "UTF7",        sizeof("UTF7")-1,        CP_UTF7 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "UTF8",        sizeof("UTF8")-1,        CP_UTF8 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "SYMBOL",      sizeof("SYMBOL")-1,      CP_SYMBOL TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM037",  sizeof("IBM037")-1,  037 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM437",  sizeof("IBM437")-1,  437 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM500",  sizeof("IBM500")-1,  500 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ASMO708", sizeof("ASMO708")-1, 708 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "DOS720",  sizeof("DOS720")-1,  720 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IMB737",  sizeof("IMB737")-1,  737 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM775",  sizeof("IBM775")-1,  775 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM850",  sizeof("IBM850")-1,  850 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM852",  sizeof("IBM852")-1,  852 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM855",  sizeof("IBM855")-1,  855 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IMB857",  sizeof("IMB857")-1,  857 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM858",  sizeof("IBM858")-1,  858 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM860",  sizeof("IBM860")-1,  860 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM861",  sizeof("IBM861")-1,  861 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "DOS862",  sizeof("DOS862")-1,  862 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM863",  sizeof("IBM863")-1,  863 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM864",  sizeof("IBM864")-1,  864 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM865",  sizeof("IBM865")-1,  865 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "CP866",   sizeof("CP866")-1,   866 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM869",  sizeof("IBM869")-1,  869 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM855",  sizeof("IBM855")-1,  855 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM870",  sizeof("IBM870")-1,  870 TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "WINDOWS_874",    sizeof("WINDOWS_874")-1,    874 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "CP875",          sizeof("CP875")-1,          875 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "SHIFT_JIS",      sizeof("SHIFT_JIS")-1,      932 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "GB2312",         sizeof("GB2312")-1,         936 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "KS_C_5601_1987", sizeof("KS_C_5601_1987")-1, 949 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "BIG5",           sizeof("BIG5")-1,           950 TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM01047", sizeof("IBM01047")-1, 1047 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM01140", sizeof("IBM01140")-1, 1140 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM01141", sizeof("IBM01141")-1, 1141 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM01142", sizeof("IBM01142")-1, 1142 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM01143", sizeof("IBM01143")-1, 1143 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM01144", sizeof("IBM01144")-1, 1144 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM01145", sizeof("IBM01145")-1, 1145 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM01146", sizeof("IBM01146")-1, 1146 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM01147", sizeof("IBM01147")-1, 1147 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM01148", sizeof("IBM01148")-1, 1148 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM01149", sizeof("IBM01149")-1, 1149 TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "WINDOWS_1250", sizeof("WINDOWS_1250")-1, 1250 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "WINDOWS_1251", sizeof("WINDOWS_1251")-1, 1251 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "WINDOWS_1252", sizeof("WINDOWS_1252")-1, 1252 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "WINDOWS_1253", sizeof("WINDOWS_1253")-1, 1253 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "WINDOWS_1254", sizeof("WINDOWS_1254")-1, 1254 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "WINDOWS_1255", sizeof("WINDOWS_1255")-1, 1255 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "WINDOWS_1256", sizeof("WINDOWS_1256")-1, 1256 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "WINDOWS_1257", sizeof("WINDOWS_1257")-1, 1257 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "WINDOWS_1258", sizeof("WINDOWS_1258")-1, 1258 TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "JOHAB", sizeof("JOHAB")-1, 1361 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "MAC",   sizeof("MAC")-1,   10000 TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_JAPANESE",    sizeof("X_MAC_JAPANESE")-1,    10001 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_CHINESETRAD", sizeof("X_MAC_CHINESETRAD")-1, 10002 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_KOREAN",      sizeof("X_MAC_KOREAN")-1,      10003 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_ARABIC",      sizeof("X_MAC_ARABIC")-1,      10004 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_HEBREW",      sizeof("X_MAC_HEBREW")-1,      10005 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_GREEK",       sizeof("X_MAC_GREEK")-1,       10006 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_CYRILLIC",    sizeof("X_MAC_CYRILLIC")-1,    10007 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_CHINESESIMP", sizeof("X_MAC_CHINESESIMP")-1, 10008 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_ROMANIAN",    sizeof("X_MAC_ROMANIAN")-1,    10010 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_UKRAINIAN",   sizeof("X_MAC_UKRAINIAN")-1,   10017 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_THAI",        sizeof("X_MAC_THAI")-1,        10021 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_CE",          sizeof("X_MAC_CE")-1,          10029 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_ICELANDIC",   sizeof("X_MAC_ICELANDIC")-1,   10079 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_TURKISH",     sizeof("X_MAC_TURKISH")-1,     10081 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_MAC_CROATION",    sizeof("X_MAC_CROATION")-1,    10082 TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "X_CHINESE_CNS",  sizeof("X_Chinese_CNS")-1,  20000 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_CP20001",      sizeof("X_CP20001")-1,      20001 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_CHINESE_ETEN", sizeof("X_CHINESE_ETEN")-1, 20002 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_CP20003",      sizeof("X_CP20003")-1,      20003 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_CP20004",      sizeof("X_CP20004")-1,      20004 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_CP20005",      sizeof("X_CP20005")-1,      20005 TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "X_IA5",           sizeof("X_IA5")-1,           20115 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_IA5_GERMAN",    sizeof("X_IA5_GERMAN")-1,    20116 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_IA5_SWEDISH",   sizeof("X_IA5_SWEDISH")-1,   20117 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_IA5_NORWEGIAN", sizeof("X_IA5_NORWEGIAN")-1, 20118 TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "US_ASCII",        sizeof("US_ASCII")-1,        20127 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_CP20261",       sizeof("X_CP20261")-1,       20261 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_CP20269",       sizeof("X_CP20269")-1,       20269 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_EBCDIC_KOREAN", sizeof("X_EBCDIC_KOREAN")-1, 20833 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM_THAI",        sizeof("IBM_THAI")-1,        20838 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "KOI8_R",          sizeof("KOI8_R")-1,          20866 TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM273", sizeof("IBM273")-1, 20273 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM277", sizeof("IBM277")-1, 20277 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM278", sizeof("IBM278")-1, 20278 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM280", sizeof("IBM280")-1, 20280 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM284", sizeof("IBM284")-1, 20284 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM285", sizeof("IBM285")-1, 20285 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM290", sizeof("IBM290")-1, 20290 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM297", sizeof("IBM297")-1, 20297 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM420", sizeof("IBM420")-1, 20420 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM423", sizeof("IBM423")-1, 20423 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM424", sizeof("IBM424")-1, 20424 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM871", sizeof("IBM871")-1, 20871 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM880", sizeof("IBM880")-1, 20880 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM905", sizeof("IBM905")-1, 20905 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "IBM924", sizeof("IBM924")-1, 20924 TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "EUC_JP",         sizeof("EUC_JP")-1,         20932 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_CP20936",      sizeof("X_CP20936")-1,      20936 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_CP20949",      sizeof("X_CP20949")-1,      20949 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "CP1025",         sizeof("CP1025")-1,         21025 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "KOI8_U",         sizeof("KOI8_U")-1,         21866 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_8859_1",     sizeof("ISO_8859_1")-1,     28591 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_8859_2",     sizeof("ISO_8859_2")-1,     28592 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_8859_3",     sizeof("ISO_8859_3")-1,     28593 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_8859_4",     sizeof("ISO_8859_4")-1,     28594 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_8859_5",     sizeof("ISO_8859_5")-1,     28595 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_8859_6",     sizeof("ISO_8859_6")-1,     28596 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_8859_7",     sizeof("ISO_8859_7")-1,     28597 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_8859_8",     sizeof("ISO_8859_8")-1,     28598 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_8859_9",     sizeof("ISO_8859_9")-1,     28599 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_8859_13",    sizeof("ISO_8859_13")-1,    28603 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_8859_15",    sizeof("ISO_8859_15")-1,    28605 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_EUROPA",       sizeof("X_EUROPA")-1,       29001 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_8859_8_I",   sizeof("ISO_8859_8_I")-1,   38598 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_2022_JP",    sizeof("ISO_2022_JP")-1,    50220 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "CSISO2022JP",    sizeof("CSISO2022JP")-1,    50221 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_2022_JP_SO", sizeof("ISO_2022_JP_SO")-1, 50222 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_2022_KR",    sizeof("ISO_2022_KR")-1,    50225 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_CP50227",      sizeof("X_CP50227")-1,      50227 TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "ISO_2022",      sizeof("ISO_2022")-1,      50229 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "EBCDIC_JP",     sizeof("EBCDIC_JP")-1,     50930 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "EBCDIC_US",     sizeof("EBCDIC_US")-1,     50931 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "EBCDIC_KR",     sizeof("EBCDIC_KR")-1,     50933 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "EBCDIC_CN_EX",  sizeof("EBCDIC_CN_EX")-1,  50935 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "EBCDIC_CN",     sizeof("EBCDIC_CN")-1,     50936 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "EBCDIC_CN_TRA", sizeof("EBCDIC_CN_TRA")-1, 50937 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "EBCDIC_JP_EX",  sizeof("EBCDIC_JP_EX")-1,  50939 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "EUC_JP",        sizeof("EUC_JP")-1,        51932 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "EUC_CN",        sizeof("EUC_CN")-1,        51936 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "EUC_KR",        sizeof("EUC_KR")-1,        51949 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "EUC_CN_TRA",    sizeof("EUC_CN_TRA")-1,    51950 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "HZ_GB_2312",    sizeof("HZ_GB_2312")-1,    52936 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "GB18030",       sizeof("GB18030")-1,       54936 TSRMLS_CC);

	zend_declare_class_constant_long(ce_winsystem_codepage, "X_ISCII_DE", sizeof("X_ISCII_DE")-1, 57002 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_ISCII_BE", sizeof("X_ISCII_BE")-1, 57003 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_ISCII_TA", sizeof("X_ISCII_TA")-1, 57004 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_ISCII_TE", sizeof("X_ISCII_TE")-1, 57005 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_ISCII_AS", sizeof("X_ISCII_AS")-1, 57006 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_ISCII_OR", sizeof("X_ISCII_OR")-1, 57007 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_ISCII_KA", sizeof("X_ISCII_KA")-1, 57008 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_ISCII_MA", sizeof("X_ISCII_MA")-1, 57009 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_ISCII_GU", sizeof("X_ISCII_GU")-1, 57010 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_codepage, "X_ISCII_PA", sizeof("X_ISCII_PA")-1, 57011 TSRMLS_CC);

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
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
  | Author: Elizabeth Smith <auroraeosrose@gmail.com>                    |
  +----------------------------------------------------------------------+
*/

#include "php_winsystem.h"

zend_class_entry *ce_winsystem_exception;
zend_class_entry *ce_winsystem_invalidargumentexception;
zend_class_entry *ce_winsystem_runtimeexception;
zend_class_entry *ce_winsystem_versionexception;
zend_class_entry *ce_winsystem_outofboundsexception;

/* ----------------------------------------------------------------
    Win\System Exception C API
------------------------------------------------------------------*/

/* {{{ winsystem_create_error - grabs a message from GetLastError int and
       throws an exception with it */
PHP_WINSYSTEM_API void winsystem_create_error(int error, zend_class_entry *ce TSRMLS_DC)
{
	DWORD ret;
	char * buffer = NULL;

	/* Get a system message from the getlasterror value, tell windows to allocate
	   the buffer, and don't sprintf in any args */
	ret = FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, 
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&buffer,
		0,
		NULL);

	if (ret == 0) {
		zend_throw_exception(ce, "Cannot retrieve error message properly", GetLastError() TSRMLS_CC);
		return;
	}

	zend_throw_exception(ce, buffer, error TSRMLS_CC);
	LocalFree(buffer);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(winsystem_exceptions)
{
	zend_class_entry ce, runtime_ce, version_ce, arg_ce, bounds_ce;

	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Exception", NULL);
	ce_winsystem_exception = zend_register_internal_class(&ce TSRMLS_CC);
	ce_winsystem_exception->ce_flags |= ZEND_ACC_INTERFACE;

	INIT_NS_CLASS_ENTRY(runtime_ce, PHP_WINSYSTEM_NS, "RuntimeException", NULL);
	ce_winsystem_runtimeexception = zend_register_internal_class_ex(&runtime_ce, spl_ce_RuntimeException, NULL TSRMLS_CC);
	zend_class_implements(ce_winsystem_runtimeexception TSRMLS_CC, 1, ce_winsystem_exception);

	INIT_NS_CLASS_ENTRY(version_ce, PHP_WINSYSTEM_NS, "VersionException", NULL);
	ce_winsystem_versionexception = zend_register_internal_class_ex(&version_ce, ce_winsystem_runtimeexception, NULL TSRMLS_CC);

	INIT_NS_CLASS_ENTRY(arg_ce, PHP_WINSYSTEM_NS, "InvalidArgumentException", NULL);
	ce_winsystem_invalidargumentexception = zend_register_internal_class_ex(&arg_ce, spl_ce_InvalidArgumentException, NULL TSRMLS_CC);
	zend_class_implements(ce_winsystem_invalidargumentexception TSRMLS_CC, 1, ce_winsystem_exception);

	INIT_NS_CLASS_ENTRY(bounds_ce, PHP_WINSYSTEM_NS, "OutOfBoundsException", NULL);
	ce_winsystem_outofboundsexception = zend_register_internal_class_ex(&bounds_ce, spl_ce_OutOfBoundsException, NULL TSRMLS_CC);
	zend_class_implements(ce_winsystem_outofboundsexception TSRMLS_CC, 1, ce_winsystem_exception);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
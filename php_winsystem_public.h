/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2011 The PHP Group                                |
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

#ifndef PHP_WINSYSTEM_API_H
#define PHP_WINSYSTEM_API_H

#ifndef PHP_WINSYSTEM_API
#define PHP_WINSYSTEM_API __declspec(dllimport)
#endif

#undef _WIN32_WINNT
#undef NTDDI_VERSION
#define _WIN32_WINNT 0x0600 /* Vista */
#define NTDDI_VERSION  NTDDI_VISTA /* Vista */

#define _WINSOCKAPI_ /* keep windows.h from spewing errors */
#include <Windows.h> /* Include this FIRST we want vista stuff! */

/* Needed PHP includes */
#include <php.h>

/* ----------------------------------------------------------------
  C APIs
------------------------------------------------------------------*/
extern PHP_WINSYSTEM_API void winsystem_create_error(int error, zend_class_entry *ce TSRMLS_DC);

extern PHP_WINSYSTEM_API WCHAR * php_winsystem_unicode_char_to_wchar(const CHAR ** utf8_string, int codepage);
extern PHP_WINSYSTEM_API CHAR * php_winsystem_unicode_wchar_to_char(const WCHAR ** utf16_string, int codepage);
extern PHP_WINSYSTEM_API WCHAR * php_winsystem_unicode_get_wchar(zval** unicodeclass TSRMLS_DC);
extern PHP_WINSYSTEM_API CHAR * php_winsystem_unicode_get_char(zval** unicodeclass TSRMLS_DC);
extern PHP_WINSYSTEM_API void php_winsystem_unicode_create(zval** object, wchar_t *text, int length TSRMLS_DC);

extern PHP_WINSYSTEM_API long php_winsystem_get_enum_value(zval** enumclass TSRMLS_DC);
extern PHP_WINSYSTEM_API void php_winsystem_set_enum_value(zval** enumclass, long value TSRMLS_DC);
extern PHP_WINSYSTEM_API int php_winsystem_enum_check_valid(long value, zend_class_entry *ce TSRMLS_DC);

/* ----------------------------------------------------------------
  Exported Class Entries
------------------------------------------------------------------*/
extern PHP_WINSYSTEM_API zend_class_entry *ce_winsystem_enum;
extern PHP_WINSYSTEM_API zend_class_entry *ce_winsystem_unicode;
extern PHP_WINSYSTEM_API zend_class_entry *ce_winsystem_runtimeexception;
extern PHP_WINSYSTEM_API zend_class_entry *ce_winsystem_invalidargumentexception;

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
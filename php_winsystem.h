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
  | Author: Elizabeth Smith <auroraeosrose@gmail.net>                    |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_WINSYSTEM_H
#define PHP_WINSYSTEM_H

#undef _WIN32_WINNT
#undef NTDDI_VERSION
#define _WIN32_WINNT 0x0600 /* Vista */
#define NTDDI_VERSION  NTDDI_VISTA /* Vista */

#define _WINSOCKAPI_ /* keep windows.h from spewing errors */
#include <Windows.h> /* Include this FIRST we want vista stuff! */

/* version info file */
#include "version.h"

/* Needed PHP includes */
#include <php.h>
#include <zend_exceptions.h>
#include <ext/spl/spl_exceptions.h>

#ifdef PHP_WIN32
#define PHP_WINSYSTEM_API __declspec(dllexport)
#else
#define PHP_WINSYSTEM_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/* Externally useable APIS*/
#include "php_winsystem_public.h"

/* ----------------------------------------------------------------
  Macros
------------------------------------------------------------------*/
#define PHP_WINSYSTEM_NS ZEND_NS_NAME("Win", "System")

#define PHP_WINSYSTEM_EXCEPTIONS \
zend_error_handling error_handling; \
zend_replace_error_handling(EH_THROW, ce_winsystem_invalidargumentexception, &error_handling TSRMLS_CC);

#define PHP_WINSYSTEM_RESTORE_ERRORS \
zend_restore_error_handling(&error_handling TSRMLS_CC);

#define REGISTER_ENUM_CONST(const_name, value, ce) \
zend_declare_class_constant_long(ce, const_name, sizeof(const_name)-1, (long)value TSRMLS_CC);

/* ----------------------------------------------------------------
  Typedefs
------------------------------------------------------------------*/
typedef struct _winsystem_enum_object winsystem_enum_object;
typedef struct _winsystem_unicode_object winsystem_unicode_object;
typedef struct _winsystem_timer_callback winsystem_timer_callback;
typedef struct _winsystem_timer_object winsystem_timer_object;
typedef struct _winsystem_waitable_object winsystem_waitable_object;
typedef struct _winsystem_event_object winsystem_event_object;
typedef struct _winsystem_mutex_object winsystem_mutex_object;
typedef struct _winsystem_semaphore_object winsystem_semaphore_object;

#ifdef ZTS
typedef struct _winsystem_thread_data winsystem_thread_data;
typedef struct _winsystem_thread_object  winsystem_thread_object;
#endif

/* Names in most things can be either a string or unicode object */
typedef union _winsystem_name {
	zval *      unicode_object;
	char *      string;
} winsystem_name;

/* ----------------------------------------------------------------
  Class Entries
------------------------------------------------------------------*/
extern zend_class_entry *ce_winsystem_invalidargumentexception;
extern zend_class_entry *ce_winsystem_codepage;
extern zend_class_entry *ce_winsystem_outofboundsexception;
extern zend_class_entry *ce_winsystem_runtimeexception;
extern zend_class_entry *ce_winsystem_event;
extern zend_class_entry *ce_winsystem_waitable;

/* ----------------------------------------------------------------
  Object Globals, lifecycle and static linking
------------------------------------------------------------------*/
ZEND_BEGIN_MODULE_GLOBALS(winsystem)
	zend_llist processes;
#ifdef ZTS
	zend_llist threads;
#endif
ZEND_END_MODULE_GLOBALS(winsystem)

#ifdef ZTS
# define WINSYSTEM_G(v)   TSRMG(winsystem_globals_id, zend_winsystem_globals*, v)
#else
# define WINSYSTEM_G(v)   (winsystem_globals.v)
#endif

PHP_MINIT_FUNCTION(winsystem_enum);
PHP_MINIT_FUNCTION(winsystem_codepage);
PHP_MINIT_FUNCTION(winsystem_unicode);
PHP_MINIT_FUNCTION(winsystem_exceptions);

PHP_MINIT_FUNCTION(winsystem_waitable);
PHP_MINIT_FUNCTION(winsystem_mutex);
PHP_MINIT_FUNCTION(winsystem_semaphore);
PHP_MINIT_FUNCTION(winsystem_event);
PHP_MINIT_FUNCTION(winsystem_timer);
PHP_MINIT_FUNCTION(winsystem_registry);

PHP_MINIT_FUNCTION(winsystem_thread);
PHP_RINIT_FUNCTION(winsystem_thread);
PHP_RSHUTDOWN_FUNCTION(winsystem_thread);
PHP_MSHUTDOWN_FUNCTION(winsystem_thread);

/* Required for static linking */
extern zend_module_entry winsystem_module_entry;
#define phpext_winsystem_ptr &winsystem_module_entry

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
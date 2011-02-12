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

#ifndef PHP_WINSYSTEM_H
#define PHP_WINSYSTEM_H

/* version info file */
#include "winsystem_version.h"

/* Needed PHP includes */
#include "php.h"

#ifdef PHP_WIN32
#define PHP_WINSYSTEM_API __declspec(dllexport)
#else
#define PHP_WINSYSTEM_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/* Externally useable APIS*/
#include "php_winsystem_api.h"

/* ----------------------------------------------------------------
  Typedefs
------------------------------------------------------------------*/
#define PHP_WINSYSTEM_NS ZEND_NS_NAME("Win", "System")
#define PHP_WINSYSTEM_SERVICE_NS ZEND_NS_NAME(PHP_WINSYSTEM_NS, "Service")

/* ----------------------------------------------------------------
  Property Magic
------------------------------------------------------------------*/

/* Property read/write callbacks */
typedef int (* winsystem_prop_read_t) (winsystem_generic_object *object, zval *member, zval **retval TSRMLS_DC);
typedef int (* winsystem_prop_write_t)(winsystem_generic_object *object, zval *member, zval *value TSRMLS_DC);

/* Container for read/write callback */
typedef struct _winsystem_prop_handler {
	winsystem_prop_read_t  read_func;
	winsystem_prop_write_t write_func;
} winsystem_prop_handler;

/* Registers the read and write handlers for a class's property */
static inline void winsystem_register_prop_handler(HashTable *prop_handlers, zend_class_entry *ce, char *prop_name, 
												   winsystem_prop_read_t read_func, winsystem_prop_write_t write_func TSRMLS_DC)
{
	winsystem_prop_handler handler;

	handler.read_func  = read_func;
	handler.write_func = write_func;

	zend_hash_add(prop_handlers, prop_name, strlen(prop_name) + 1, &handler, sizeof(winsystem_prop_handler), NULL);
	zend_declare_property_null(ce, prop_name, strlen(prop_name), ZEND_ACC_PUBLIC TSRMLS_CC);
}

/* ----------------------------------------------------------------
  Class Entries
------------------------------------------------------------------*/
extern zend_class_entry *ce_winsystem_event;
extern zend_class_entry *ce_winsystem_waitable;
extern zend_class_entry *ce_winsystem_exception;
extern zend_class_entry *ce_winsystem_argexception;
extern zend_class_entry *ce_winsystem_versionexception;

extern zend_class_entry *ce_winsystem_service_controller;
extern zend_object_handlers winsystem_object_handlers;

/* ----------------------------------------------------------------
  Object Globals, lifecycle and static linking
------------------------------------------------------------------*/
ZEND_BEGIN_MODULE_GLOBALS(winsystem)
	zend_llist processes;
	zend_llist threads;
	DWORD process_id;
	DWORD thread_id;
ZEND_END_MODULE_GLOBALS(winsystem)

#ifdef ZTS
# define WINSYSTEM_G(v)   TSRMG(winsystem_globals_id, zend_winsystem_globals*, v)
#else
# define WINSYSTEM_G(v)   (winsystem_globals.v)
#endif

PHP_MINIT_FUNCTION(winsystem_util);
PHP_MINIT_FUNCTION(winsystem_waitable);
PHP_MINIT_FUNCTION(winsystem_mutex);
PHP_MINIT_FUNCTION(winsystem_semaphore);
PHP_MINIT_FUNCTION(winsystem_event);
PHP_MINIT_FUNCTION(winsystem_timer);
PHP_MINIT_FUNCTION(winsystem_timerqueue);
PHP_MINIT_FUNCTION(winsystem_thread);
PHP_MINIT_FUNCTION(winsystem_unicode);
PHP_MINIT_FUNCTION(winsystem_registry);

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
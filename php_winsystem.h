/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 2007-2008 Elizabeth M. Smith, Sara Golemon, Tom Rogers |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Elizabeth M. Smith <auroraeosrose@php.net>                   |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_WINSYSTEM_H
#define PHP_WINSYSTEM_H
/* version info file */
#include "winsystem_version.h"

/* Needed PHP includes */
#include "php.h"
#include <Windows.h>

#ifdef PHP_WIN32
#define PHP_WINSYSTEM_API __declspec(dllexport)
#else
#define PHP_WINSYSTEM_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/* ----------------------------------------------------------------
  Typedefs                                               
------------------------------------------------------------------*/
#define PHP_WINSYSTEM_NS ZEND_NS_NAME("Win", "System")
#define PHP_WINSYSTEM_SERVICE_NS ZEND_NS_NAME(PHP_WINSYSTEM_NS, "Service")

/* Generic object - all object's that want sexy property read/write callbacks should have this header */
typedef struct _winsystem_generic_object {
    zend_object        std;
    zend_object_handle zobject_handle;
    HashTable          *prop_handler;
} winsystem_generic_object;

/* Names in most things can be either a string or unicode object */
typedef union _winsystem_name {
	zval *      unicode_object;
	char *      string;
} winsystem_name;

/* unicode object */
typedef struct _winsystem_unicode_object {
	zend_object  std;
	zend_bool    is_constructed;
	WCHAR *      unicode_string;
	CHAR *       multibyte_string;
} winsystem_unicode_object;

/* mutex object */
typedef struct _winsystem_mutex_object {
	zend_object    std;
	zend_bool      is_constructed;
	HANDLE         handle;
	BOOL           is_owned;
	BOOL           can_inherit;
	zend_bool      is_unicode;
	winsystem_name name;
} winsystem_mutex_object;

/* semaphore object */
typedef struct _winsystem_semaphore_object {
	zend_object    std;
	zend_bool      is_constructed;
	HANDLE         handle;
	long           max_count;
	BOOL           can_inherit;
	zend_bool      is_unicode;
	winsystem_name name;
} winsystem_semaphore_object;

/* event object */
typedef struct _winsystem_event_object {
	zend_object    std;
	zend_bool      is_constructed;
	HANDLE         handle;
	BOOL           auto_reset;
	BOOL           can_inherit;
	zend_bool      is_unicode;
	winsystem_name name;
} winsystem_event_object;

/* timer object */
typedef struct _winsystem_timer_object {
	zend_object    std;
	zend_bool      is_constructed;
	HANDLE         handle;
	BOOL           auto_reset;
	BOOL           can_inherit;
	winsystem_name name;
} winsystem_timer_object;

/* Data structure for threads information */
typedef struct _winsystem_thread_data {
	char *   src_filename;
	uint     src_lineno;
	char *   file;
	void *** parent_tsrmls;
	HANDLE   start_event;
	HANDLE   thread_handle;
	DWORD    thread_id;
	void *** child_tsrmls;
	char *   classname;
	int      classlen;
} winsystem_thread_data;

/* thread object */
typedef struct _winsystem_thread_object {
	zend_object  std;
	zend_bool    is_constructed;
	HANDLE       handle;
} winsystem_thread_object;

/* Property read/write callbacks */
typedef int (* winsystem_prop_read_t) (winsystem_generic_object *object, zval *member, zval **retval TSRMLS_DC);
typedef int (* winsystem_prop_write_t)(winsystem_generic_object *object, zval *member, zval *value TSRMLS_DC);

/* Container for read/write callback */
typedef struct _winsystem_prop_handler {
    winsystem_prop_read_t  read_func;
    winsystem_prop_write_t write_func;
} winsystem_prop_handler;

/* ----------------------------------------------------------------
  Exported C API                                            
------------------------------------------------------------------*/

extern PHP_WINSYSTEM_API zend_class_entry *ce_winsystem_unicode;
extern PHP_WINSYSTEM_API void winsystem_create_error(int error, zend_class_entry *ce TSRMLS_DC);
extern PHP_WINSYSTEM_API WCHAR * win_system_convert_to_wchar(const CHAR ** utf8_string, int type);
extern PHP_WINSYSTEM_API CHAR * win_system_convert_to_char(const WCHAR ** utf16_string, int type);

/* ----------------------------------------------------------------
  Class Entries                                              
------------------------------------------------------------------*/
extern zend_class_entry *ce_winsystem_waitable;
extern zend_class_entry *ce_winsystem_exception;
extern zend_class_entry *ce_winsystem_argexception;
extern zend_class_entry *ce_winsystem_versionexception;

extern zend_class_entry *ce_winsystem_service_controller;

/* ----------------------------------------------------------------
  Internal APIs                                              
------------------------------------------------------------------*/
extern int unset_abstract_flag(zend_function *func TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key);
extern zend_object_handlers winsystem_object_handlers;

/* Registers the read and write handlers for a class's property */
static inline void winsystem_register_prop_handler(HashTable *prop_handlers, zend_class_entry *ce, char *prop_name, 
                                                   winsystem_prop_read_t read_func, winsystem_prop_write_t write_func TSRMLS_DC)
{
    winsystem_prop_handler handler;

    handler.read_func  = read_func;
    handler.write_func = write_func;

    zend_hash_add(prop_handlers, prop_name, sizeof(prop_name) - 1, &handler, sizeof(winsystem_prop_handler), NULL);
    zend_declare_property_null(ce, prop_name, sizeof(prop_name) - 1, ZEND_ACC_PUBLIC TSRMLS_CC);
}

/* ----------------------------------------------------------------
  Object Globals, lifecycle and static linking                                                
------------------------------------------------------------------*/
ZEND_BEGIN_MODULE_GLOBALS(winsystem)
	zend_llist threads;
    zend_llist processes;
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
PHP_MINIT_FUNCTION(winsystem_thread);
PHP_MINIT_FUNCTION(winsystem_unicode);
PHP_MINIT_FUNCTION(winsystem_service);
PHP_MINIT_FUNCTION(winsystem_service_controller);

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
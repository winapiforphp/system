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

/* $Id$ */

#ifndef PHP_WINSYSTEM_H
#define PHP_WINSYSTEM_H

/* In order to allow newer features tell the compiler we're the same as the sdk default version */
#define _WIN32_WINNT                     0x0600
#define _WIN32_IE                        0x0600

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

/* ----------------------------------------------------------------
  Typedefs                                               
------------------------------------------------------------------*/
#define PHP_WINSYSTEM_NS ZEND_NS_NAME("Win", "System")

/* unicode object */
typedef struct _winsystem_unicode_object {
	zend_object  std;
	WCHAR * unicode_string;
	CHAR * multibyte_string;
} winsystem_unicode_object;

/* mutex object */
typedef struct _winsystem_mutex_object {
	zend_object  std;
	zend_object_handle handle;
	HANDLE handle_object;
	BOOL is_owned;
	BOOL can_inherit;
	zval * unicode;
	char * name;
} winsystem_mutex_object;

/* semaphore object */
typedef struct _winsystem_semaphore_object {
	zend_object  std;
	zend_object_handle handle;
	HANDLE handle_object;
	long count;
	long max_count;
	BOOL can_inherit;
	char * name;
} winsystem_semaphore_object;

/* event object */
typedef struct _winsystem_event_object {
	zend_object  std;
	zend_object_handle handle;
	HANDLE handle_object;
	BOOL auto_reset;
	BOOL can_inherit;
	char * name;
} winsystem_event_object;

/* timer object */
typedef struct _winsystem_timer_object {
	zend_object  std;
	zend_object_handle handle;
	HANDLE handle_object;
	BOOL auto_reset;
	BOOL can_inherit;
	char * name;
} winsystem_timer_object;

/* Data structure for threads information */
typedef struct _winsystem_thread_data {
	char *src_filename;
	uint src_lineno;
	char * file;
	void ***parent_tsrmls;
	HANDLE start_event;
	HANDLE thread_handle;
	DWORD thread_id;
	void ***child_tsrmls;
	char *classname;
	int classlen;
} winsystem_thread_data;

/* thread object */
typedef struct _winsystem_thread_object {
	zend_object  std;
	zend_object_handle handle;
	HANDLE handle_object;
} winsystem_thread_object;

int unset_abstract_flag(zend_function *func TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key);
void winsystem_create_error(int error, zend_class_entry *ce TSRMLS_DC);

/* ----------------------------------------------------------------
  Class Entries                                              
------------------------------------------------------------------*/
extern zend_class_entry *ce_winsystem_waitable;
extern zend_class_entry *ce_winsystem_exception;

/* Exported class entry */
extern PHP_WINSYSTEM_API zend_class_entry *ce_winsystem_unicode;
extern inline winsystem_unicode_object* winsystem_unicode_object_get(zval *zobj TSRMLS_DC);

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
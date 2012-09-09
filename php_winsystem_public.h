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

#define PHP_WINSYSTEM_NS ZEND_NS_NAME("Win", "System")
#define PHP_WINSYSTEM_SERVICE_NS ZEND_NS_NAME(PHP_WINSYSTEM_NS, "Service")

/* ----------------------------------------------------------------
  Typedefs
------------------------------------------------------------------*/

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

/* waitable object */
typedef struct _winsystem_waitable_object {
	zend_object  std;
	zend_bool    is_constructed;
	HANDLE       handle;
} winsystem_waitable_object;

/* mutex object */
typedef struct _winsystem_mutex_object {
	zend_object    std;
	zend_bool      is_constructed;
	HANDLE         handle;
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
	BOOL           can_inherit;
	zend_bool      is_unicode;
	winsystem_name name;
} winsystem_event_object;

/* Storage container for timer callbacks */
typedef struct _winsystem_timer_callback {
	zend_fcall_info callback_info;
	zend_fcall_info_cache callback_cache;
	char *src_filename;
	uint src_lineno;
	int refcount;
#ifdef ZTS
	TSRMLS_D;
#endif
} winsystem_timer_callback;

/* timer object */
typedef struct _winsystem_timer_object {
	zend_object    std;
	zend_bool      is_constructed;
	HANDLE         handle;
	BOOL           can_inherit;
	zend_bool      is_unicode;
	winsystem_name name;
	winsystem_timer_callback *store;
} winsystem_timer_object;

typedef struct _winsystem_service_controller_object {
	zend_object    std;
	zend_bool      is_constructed;
} winsystem_service_controller_object;

#ifdef ZTS

/* timerqueue object */
typedef struct _winsystem_timerqueue_object {
	zend_object    std;
	zend_bool      is_constructed;
	HANDLE         handle;
	zval           *event;
	HashTable      *timers;
} winsystem_timerqueue_object;

/* Data structure for threads information */
typedef struct _winsystem_thread_data {
	char *   file;
	void *** parent_tsrmls;
	HANDLE   start_event;
	HANDLE   thread_handle;
	DWORD    thread_id;
	zend_fcall_info callback_info;
} winsystem_thread_data;

/* thread object */
typedef struct _winsystem_thread_object {
	zend_object  std;
	zend_bool    is_constructed;
	HANDLE       handle;
} winsystem_thread_object;

#endif

/* ----------------------------------------------------------------
  C APIs
------------------------------------------------------------------*/
extern PHP_WINSYSTEM_API void winsystem_create_error(int error, zend_class_entry *ce TSRMLS_DC);
extern PHP_WINSYSTEM_API int winsystem_juggle_type(zval *value, int type TSRMLS_DC);
extern PHP_WINSYSTEM_API WCHAR * php_winsystem_convert_to_wchar(const CHAR ** utf8_string, int type);
extern PHP_WINSYSTEM_API CHAR * php_winsystem_convert_to_char(const WCHAR ** utf16_string, int type);

extern PHP_WINSYSTEM_API long php_winsystem_get_enum_value(zval** enumclass TSRMLS_DC);
extern PHP_WINSYSTEM_API void php_winsystem_set_enum_value(zval** enumclass, long value TSRMLS_DC);
extern PHP_WINSYSTEM_API WCHAR * php_winsystem_get_unicode_wchar(zval** unicodeclass TSRMLS_DC);

/* ----------------------------------------------------------------
  Exported Class Entries
------------------------------------------------------------------*/
extern PHP_WINSYSTEM_API zend_class_entry *ce_winsystem_enum;
extern PHP_WINSYSTEM_API zend_class_entry *ce_winsystem_unicode;

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
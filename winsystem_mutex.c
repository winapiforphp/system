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

/* All the classes in this file */
zend_class_entry *ce_winsystem_mutex;

static inline winsystem_mutex_object* winsystem_mutex_object_get(zval *zobj TSRMLS_DC)
{
    winsystem_mutex_object *pobj = zend_object_store_get_object(zobj TSRMLS_CC);
    if (pobj->handle_object == NULL) {
		php_error(E_ERROR, "Internal mutex handle missing in %s class, you must call parent::__construct in extended classes", Z_OBJCE_P(zobj)->name);
		return NULL;
    }
    return pobj;
}

/* ----------------------------------------------------------------
  Win\System\Mutex Userland API                                                    
------------------------------------------------------------------*/
ZEND_BEGIN_ARG_INFO_EX(WinSystemMutex___construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, own)
    ZEND_ARG_INFO(0, process_inherit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemMutex_release_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemMutex_isOwned_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemMutex_getName_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemMutex_canInherit_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemMutex_wait_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

/* {{{ proto bool Win\System\Mutex->__construct()
       creates a new mutex, optionally sets the owner to the thread that called it
	   if the mutex cannot be created, will attempt to open it instead
	   if ownership is requested, and the mutex cannot be owned this will throw an exception */
PHP_METHOD(WinSystemMutex, __construct)
{
	/* Used for multibyte string */
	char * name = NULL;
	int name_length;

	/* Used for widechar string */
	zval *unicode = NULL;
	winsystem_unicode_object *unicode_object = NULL;
	int use_unicode = 0;

	/* Variables for a and w versions */
	SECURITY_ATTRIBUTES mutex_attributes;
	zend_bool inherit = TRUE;
	zend_bool own = FALSE;
	HANDLE mutex_handle;
	zend_error_handling error_handling;
	winsystem_mutex_object *mutex = (winsystem_mutex_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	/* version one, use unicode */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|bb", &unicode, ce_winsystem_unicode, &own, &inherit) != FAILURE) {
		use_unicode = 1;
		unicode_object = (winsystem_unicode_object *)winsystem_unicode_object_get(unicode TSRMLS_CC);
	} else if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!bb", &name, &name_length, &own, &inherit) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	mutex_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	mutex_attributes.lpSecurityDescriptor = NULL;
	mutex_attributes.bInheritHandle = inherit;

	if (use_unicode) {
		mutex_handle = CreateMutexW(&mutex_attributes, own, unicode_object->unicode_string);
	} else {
		mutex_handle = CreateMutexA(&mutex_attributes, own, name);
	}
	
	if(own && GetLastError() == ERROR_ALREADY_EXISTS) {
		zend_throw_exception(ce_winsystem_exception, "Mutex could not be created and marked as owned", 0 TSRMLS_CC);
		return;
	}

	/* we couldn't open it, try openmutex */
	if (mutex_handle == NULL) {
		if (use_unicode) {
			mutex_handle = OpenMutexW(SYNCHRONIZE, inherit, unicode_object->unicode_string);
		} else {
			mutex_handle = OpenMutexA(SYNCHRONIZE, inherit, name);
		}
	}

	/* mutex is STILL null, we couldn't create the mutex, fail */
	if (mutex_handle == NULL) {
		zend_throw_exception(ce_winsystem_exception, "Could not create or open the requested mutex", 0 TSRMLS_CC);
		return;
	}

	mutex->handle_object = mutex_handle;
	mutex->is_owned = own;
	mutex->can_inherit = inherit;
	if(name) {
		mutex->name = estrdup(name);
	} else if (use_unicode) {
		/* ref our object and store it */
		mutex->name = estrdup(unicode_object->multibyte_string);
	}
}
/* }}} */

/* {{{ proto bool Win\System\Mutex->getName()
       returns the current name of the mutex or null if it's unnamed */
PHP_METHOD(WinSystemMutex, getName)
{
	zend_error_handling error_handling;
	winsystem_mutex_object *mutex = (winsystem_mutex_object*)winsystem_mutex_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (mutex->name) {
		RETURN_STRING(mutex->name, 1)
	}
	RETURN_NULL()
}
/* }}} */

/* {{{ proto bool Win\System\Mutex->release()
       releases a mutex - will fail if the mutex is not owned */
PHP_METHOD(WinSystemMutex, release)
{
	BOOL worked;
	zend_error_handling error_handling;
	winsystem_mutex_object *mutex = (winsystem_mutex_object *)winsystem_mutex_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	worked = ReleaseMutex(mutex->handle_object);
	if (worked == 0) {
		RETURN_FALSE
	}
	mutex->is_owned = FALSE;

	RETURN_TRUE
}
/* }}} */

/* {{{ proto bool Win\System\Mutex->isOwned()
       mutex is owned if the thread did a wait on it or created it with the "owned" flag */
PHP_METHOD(WinSystemMutex, isOwned)
{
	zend_error_handling error_handling;
	winsystem_mutex_object *mutex = (winsystem_mutex_object *)winsystem_mutex_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(mutex->is_owned)
}
/* }}} */

/* {{{ proto bool Win\System\Mutex->canInherit()
      if a mutex was created with a "can inherit" flag, meaning child processes can grab it */
PHP_METHOD(WinSystemMutex, canInherit)
{
	zend_error_handling error_handling;
	winsystem_mutex_object *mutex = (winsystem_mutex_object *)winsystem_mutex_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(mutex->can_inherit)
}
/* }}} */

/* {{{ proto bool Win\System\Mutex->wait([int $milliseconds, bool $alertable])
        checks the current state of the specified mutex
		If the object's state is nonsignaled, the calling thread waits until the object is signaled
		or the time-out interval elapses. */
PHP_METHOD(WinSystemMutex, wait)
{
	DWORD ret;
	long milliseconds = INFINITE;
	winsystem_mutex_object *object = (winsystem_mutex_object *)winsystem_mutex_object_get(getThis() TSRMLS_CC);

	// TODO: parse parameters here and support EX

	if (object->handle_object == NULL) {
		//zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Missing internal data, remember to call parent::__construct in extended classes");
		return;
	}

	ret = WaitForSingleObject(object->handle_object, milliseconds);
	object->is_owned = TRUE;
	RETURN_LONG(ret);
}
/* }}} */

/* register mutex methods */
static zend_function_entry winsystem_mutex_functions[] = {
	PHP_ME(WinSystemMutex, __construct, WinSystemMutex___construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(WinSystemMutex, release, WinSystemMutex_release_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemMutex, isOwned, WinSystemMutex_isOwned_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemMutex, getName, WinSystemMutex_getName_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemMutex, canInherit, WinSystemMutex_canInherit_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemMutex, wait, WinSystemMutex_wait_args, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Mutex Object Magic LifeCycle Functions                                                    
------------------------------------------------------------------*/

/* cleans up the handle object */
static void winsystem_mutex_object_destructor(void *object TSRMLS_DC)
{
	winsystem_mutex_object *mutex_object = (winsystem_mutex_object *)object;

	zend_object_std_dtor(&mutex_object->std TSRMLS_CC);

	/* someone left an owned mutex unreleased - BAD BAD */
	if(mutex_object->is_owned) {
		ReleaseMutex(mutex_object->handle_object);
	}
	if(mutex_object->name) {
		efree(mutex_object->name);
	}
	if(mutex_object->handle_object != NULL){
		CloseHandle(mutex_object->handle_object);
	}
	
	efree(object);
}
/* }}} */

/* {{{ winsystem_mutex_object_new
       object that has an internal HANDLE stored */
static zend_object_value winsystem_mutex_object_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	winsystem_mutex_object *mutex_object;
	zval *tmp;

	mutex_object = ecalloc(1, sizeof(winsystem_mutex_object));
	mutex_object->handle_object = NULL;
	mutex_object->is_owned = FALSE;
	mutex_object->can_inherit = FALSE;
	mutex_object->name = NULL;
	mutex_object->unicode = NULL;

	zend_object_std_init(&mutex_object->std, ce TSRMLS_CC);
	zend_hash_copy(mutex_object->std.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(mutex_object, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) winsystem_mutex_object_destructor, NULL TSRMLS_CC);
	mutex_object->handle = retval.handle;
	retval.handlers = zend_get_std_object_handlers();
	return retval;
}
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Mutex LifeCycle Functions                                                    
------------------------------------------------------------------*/
PHP_MINIT_FUNCTION(winsystem_mutex)
{
	zend_class_entry ce;
	
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Mutex", winsystem_mutex_functions);
	ce_winsystem_mutex = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(ce_winsystem_mutex TSRMLS_CC, 1, ce_winsystem_waitable);
	ce_winsystem_mutex->create_object = winsystem_mutex_object_new;

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
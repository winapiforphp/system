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

/* {{{ proto bool Win\System\Thread->__construct()
       creates a new mutex, optionally sets the owner to the thread that called it
	   if the mutex cannot be created, will attempt to open it instead
	   if ownership is requested, and the mutex cannot be owned this will throw an exception */
PHP_METHOD(WinSystemMutex, __construct)
{
	SECURITY_ATTRIBUTES mutex_attributes;
	zend_bool inherit = TRUE;
	zend_bool own = FALSE;
	char * name = NULL;
	int name_length, unicode_length;
	LPWSTR unicodestr = NULL;
	HANDLE mutex_handle;
	zend_error_handling error_handling;
	winsystem_handle_object *mutex = (winsystem_handle_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!bb", &name, &name_length, &own, &inherit) == FAILURE) {
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (name) {
		if (FAILURE  == winsystem_ansi_to_unicode(name, name_length, &unicodestr, &unicode_length TSRMLS_CC)) {
			zend_throw_exception(ce_winsystem_exception, "Invalid name provided for mutex", 0 TSRMLS_CC);
			return;
		}
	}

	mutex_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	mutex_attributes.lpSecurityDescriptor = NULL;
	mutex_attributes.bInheritHandle = inherit;

	mutex_handle = CreateMutex(&mutex_attributes, own, unicodestr);

	if(own && GetLastError() == ERROR_ALREADY_EXISTS) {
		zend_throw_exception(ce_winsystem_exception, "Mutex could not be created and marked as owned", 0 TSRMLS_CC);
		return;
	}

	/* we couldn't open it, try openmutex */
	if (mutex_handle == NULL) {
		mutex_handle = OpenMutex(SYNCHRONIZE, inherit, unicodestr);
	}

	/* mutex is STILL null, we couldn't create the mutex, fail */
	if (mutex_handle == NULL) {
		zend_throw_exception(ce_winsystem_exception, "Could not create or open the requested mutex", 0 TSRMLS_CC);
		return;
	}

	mutex->handle_object = mutex_handle;
	mutex->is_owned = own;
	mutex->is_mutex = TRUE;
	if (unicodestr) {
		efree(unicodestr);
	}
}
/* }}} */

/* {{{ proto bool Win\System\Mutex->release()
       releases a mutex - will fail if the mutex is not owned */
PHP_METHOD(WinSystemMutex, release)
{
	BOOL worked;
	zend_error_handling error_handling;
	winsystem_handle_object *mutex = (winsystem_handle_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (mutex->handle_object == NULL) {
		zend_throw_exception(ce_winsystem_exception, "Missing internal data, remember to call parent::__construct in extended classes", 0 TSRMLS_CC);
		return;
	}

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
	winsystem_handle_object *mutex = (winsystem_handle_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);


	if (mutex->handle_object == NULL) {
		zend_throw_exception(ce_winsystem_exception, "Missing internal data, remember to call parent::__construct in extended classes", 0 TSRMLS_CC);
		return;
	}

	RETURN_BOOL(mutex->is_owned)
}
/* }}} */

/* register mutex methods */
static zend_function_entry winsystem_mutex_functions[] = {
	PHP_ME(WinSystemMutex, __construct, WinSystemMutex___construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(WinSystemMutex, release, WinSystemMutex_release_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemMutex, isOwned, WinSystemMutex_isOwned_args, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System Objects LifeCycle Functions                                                    
------------------------------------------------------------------*/
PHP_MINIT_FUNCTION(winsystem_objects)
{
	zend_class_entry ce;
	
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Mutex", winsystem_mutex_functions);
	ce_winsystem_mutex = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(ce_winsystem_mutex TSRMLS_CC, 1, ce_winsystem_waitable);
    zend_hash_apply_with_arguments(&ce_winsystem_mutex->function_table TSRMLS_CC, (apply_func_args_t) unset_abstract_flag, 1, ce_winsystem_waitable);
	ce_winsystem_mutex->ce_flags &= ~ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;
	ce_winsystem_mutex->create_object = winsystem_handle_object_new;

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
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
zend_class_entry *ce_winsystem_timer;

static inline winsystem_timer_object* winsystem_timer_object_get(zval *zobj TSRMLS_DC)
{
    winsystem_timer_object *pobj = zend_object_store_get_object(zobj TSRMLS_CC);
    if (pobj->handle_object == NULL) {
		php_error(E_ERROR, "Internal timer handle missing in %s class, you must call parent::__construct in extended classes", Z_OBJCE_P(zobj)->name);
		return NULL;
    }
    return pobj;
}

/* ----------------------------------------------------------------
  Win\System\Timer Userland API                                                    
------------------------------------------------------------------*/
ZEND_BEGIN_ARG_INFO_EX(WinSystemTimer___construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, auto_reset)
    ZEND_ARG_INFO(0, process_inherit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemTimer_cancel_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemTimer_set_args, ZEND_SEND_BY_VAL)
	ZEND_ARG_INFO(0, due)
	ZEND_ARG_INFO(0, period)
    ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemTimer_isAutoReset_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemTimer_getName_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemTimer_canInherit_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemTimer_wait_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

/* {{{ proto bool Win\System\Timer->__construct()
       creates a new timer */
PHP_METHOD(WinSystemTimer, __construct)
{
	SECURITY_ATTRIBUTES timer_attributes;
	zend_bool inherit = TRUE;
	zend_bool autoreset = TRUE;
	char * name = NULL;
	int name_length;
	HANDLE timer_handle;
	zend_error_handling error_handling;
	winsystem_timer_object *timer = (winsystem_timer_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!bb", &name, &name_length, &autoreset, &inherit) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	timer_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	timer_attributes.lpSecurityDescriptor = NULL;
	timer_attributes.bInheritHandle = inherit;

	timer_handle = CreateWaitableTimer(&timer_attributes, autoreset, name);

	/* we couldn't create it, try to open it */
	if (timer_handle == NULL) {
		timer_handle = OpenWaitableTimer(SYNCHRONIZE | TIMER_MODIFY_STATE, inherit, name);
	}

	/* timer is STILL null, we couldn't create the timer, fail */
	if (timer_handle == NULL) {
		zend_throw_exception(ce_winsystem_exception, "Could not create or open the requested timer", 0 TSRMLS_CC);
		return;
	}

	timer->handle_object = timer_handle;
	timer->auto_reset = autoreset;
	timer->can_inherit = inherit;
	if(name) {
		timer->name = estrdup(name);
	}
}
/* }}} */

/* {{{ proto bool Win\System\Timer->getName()
       returns the current name of the timer or null if it's unnamed */
PHP_METHOD(WinSystemTimer, getName)
{
	zend_error_handling error_handling;
	winsystem_timer_object *timer = (winsystem_timer_object*)winsystem_timer_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (timer->name) {
		RETURN_STRING(timer->name, 1)
	}
	RETURN_NULL()
}
/* }}} */

/* {{{ proto bool Win\System\Timer->cancel()
       Sets the specified waitable timer to the inactive state */
PHP_METHOD(WinSystemTimer, cancel)
{
	zend_error_handling error_handling;
	winsystem_timer_object *timer = (winsystem_timer_object*)winsystem_timer_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(CancelWaitableTimer(timer->handle_object));
}
/* }}} */

/* {{{ proto bool Win\System\Timer->set()
       Adds a callback to the waitable timer */
PHP_METHOD(WinSystemTimer, set)
{
	zend_error_handling error_handling;
	winsystem_timer_object *timer = (winsystem_timer_object*)winsystem_timer_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	//TODO: make this work
}
/* }}} */

/* {{{ proto bool Win\System\Timer->canInherit()
      if a timer was created with a "can inherit" flag, meaning child processes can grab it */
PHP_METHOD(WinSystemTimer, canInherit)
{
	zend_error_handling error_handling;
	winsystem_timer_object *timer = (winsystem_timer_object *)winsystem_timer_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(timer->can_inherit)
}
/* }}} */

/* {{{ proto bool Win\System\Timer->isAutoReset()
      if a timer was created with the autoreset flag, meaning the timer is automatically reset */
PHP_METHOD(WinSystemTimer, isAutoReset)
{
	zend_error_handling error_handling;
	winsystem_timer_object *timer = (winsystem_timer_object *)winsystem_timer_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(timer->auto_reset)
}
/* }}} */

/* {{{ proto bool Win\System\Timer->wait([int $milliseconds, bool $alertable])
        checks the current state of the specified mutex
		If the object's state is nonsignaled, the calling thread waits until the object is signaled
		or the time-out interval elapses. */
PHP_METHOD(WinSystemTimer, wait)
{
	DWORD ret;
	long milliseconds = INFINITE;
	winsystem_timer_object *timer = (winsystem_timer_object *)winsystem_timer_object_get(getThis() TSRMLS_CC);

	// TODO: parse parameters here and support EX

	if (timer->handle_object == NULL) {
		//zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Missing internal data, remember to call parent::__construct in extended classes");
		return;
	}

	ret = WaitForSingleObject(timer->handle_object, milliseconds);
	RETURN_LONG(ret);
}
/* }}} */

/* register timer methods */
static zend_function_entry winsystem_timer_functions[] = {
	PHP_ME(WinSystemTimer, __construct, WinSystemTimer___construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(WinSystemTimer, cancel, WinSystemTimer_set_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemTimer, set, WinSystemTimer_set_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemTimer, getName, WinSystemTimer_getName_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemTimer, canInherit, WinSystemTimer_canInherit_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemTimer, isAutoReset, WinSystemTimer_isAutoReset_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemTimer, wait, WinSystemTimer_wait_args, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Timer Object Magic LifeCycle Functions                                                    
------------------------------------------------------------------*/

/* cleans up the handle object */
static void winsystem_timer_object_destructor(void *object TSRMLS_DC)
{
	winsystem_timer_object *timer_object = (winsystem_timer_object *)object;

	zend_object_std_dtor(&timer_object->std TSRMLS_CC);

	if(timer_object->name) {
		efree(timer_object->name);
	}
	if(timer_object->handle_object != NULL){
		CloseHandle(timer_object->handle_object);
	}
	
	efree(object);
}
/* }}} */

/* {{{ winsystem_timer_object_new
       object that has an internal HANDLE stored */
static zend_object_value winsystem_timer_object_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	winsystem_timer_object *timer_object;
	zval *tmp;

	timer_object = emalloc(sizeof(winsystem_mutex_object));
	memset(&timer_object->std, 0, sizeof(zend_object));
	timer_object->handle_object = NULL;
	timer_object->auto_reset = FALSE;
	timer_object->can_inherit = FALSE;
	timer_object->name = NULL;

	zend_object_std_init(&timer_object->std, ce TSRMLS_CC);
	zend_hash_copy(timer_object->std.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(timer_object, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) winsystem_timer_object_destructor, NULL TSRMLS_CC);
	timer_object->handle = retval.handle;
	retval.handlers = zend_get_std_object_handlers();
	return retval;
}
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Timer LifeCycle Functions                                                    
------------------------------------------------------------------*/
PHP_MINIT_FUNCTION(winsystem_timer)
{
	zend_class_entry ce;
	
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Timer", winsystem_timer_functions);
	ce_winsystem_timer = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(ce_winsystem_timer TSRMLS_CC, 1, ce_winsystem_waitable);
	ce_winsystem_timer->create_object = winsystem_timer_object_new;

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
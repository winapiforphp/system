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
zend_class_entry *ce_winsystem_event;

/* ----------------------------------------------------------------
  Win\System\Event Userland API                                                    
------------------------------------------------------------------*/
ZEND_BEGIN_ARG_INFO_EX(WinSystemEvent___construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, initial_state)
	ZEND_ARG_INFO(0, auto_reset)
    ZEND_ARG_INFO(0, process_inherit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemEvent_reset_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemEvent_set_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemEvent_pulse_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemEvent_isAutoReset_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemEvent_getName_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemEvent_canInherit_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemEvent_wait_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

/* {{{ proto bool Win\System\Event->__construct()
       creates a new event */
PHP_METHOD(WinSystemEvent, __construct)
{
	/* Used for multibyte string */
	char * name = NULL;
	int name_length;

	/* Used for widechar string */
	zval *unicode = NULL;
	winsystem_unicode_object *unicode_object = NULL;
	int use_unicode = 0;

	SECURITY_ATTRIBUTES event_attributes;
	zend_bool inherit = TRUE;
	zend_bool state = FALSE;
	zend_bool autoreset = TRUE;
	HANDLE event_handle;
	zend_error_handling error_handling;
	winsystem_event_object *event = (winsystem_event_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	/* version one, use unicode */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|bbb", &unicode, ce_winsystem_unicode, &state, &autoreset, &inherit) != FAILURE) {
		use_unicode = 1;
		unicode_object = (winsystem_unicode_object *)winsystem_unicode_object_get(unicode TSRMLS_CC);
	} else if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!bbb", &name, &name_length, &state, &autoreset, &inherit) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	event_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	event_attributes.lpSecurityDescriptor = NULL;
	event_attributes.bInheritHandle = inherit;

	event_handle = CreateEvent(&event_attributes, state, autoreset, name);

	/* we couldn't create it, try to open it */
	if (event_handle == NULL) {
		event_handle = OpenEvent(SYNCHRONIZE, inherit, name);
	}

	/* event is STILL null, we couldn't create the event, fail */
	if (event_handle == NULL) {
		zend_throw_exception(ce_winsystem_exception, "Could not create or open the requested event", 0 TSRMLS_CC);
		return;
	}

	event->handle_object = event_handle;
	event->auto_reset = autoreset;
	event->can_inherit = inherit;
	if(name) {
		event->name = estrdup(name);
	}
}
/* }}} */

/* {{{ proto bool Win\System\Event->getName()
       returns the current name of the event or null if it's unnamed */
PHP_METHOD(WinSystemEvent, getName)
{
	zend_error_handling error_handling;
	winsystem_event_object *event = (winsystem_event_object*)winsystem_event_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (event->name) {
		RETURN_STRING(event->name, 1)
	}
	RETURN_NULL()
}
/* }}} */

/* {{{ proto bool Win\System\Event->reset()
       resets an event - not needed for autoreset signals */
PHP_METHOD(WinSystemEvent, reset)
{
	zend_error_handling error_handling;
	winsystem_event_object *event = (winsystem_event_object *)winsystem_event_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(ResetEvent(event->handle_object))
}
/* }}} */

/* {{{ proto bool Win\System\Event->set()
       sets event to "signaled" state */
PHP_METHOD(WinSystemEvent, set)
{
	zend_error_handling error_handling;
	winsystem_event_object *event = (winsystem_event_object *)winsystem_event_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(SetEvent(event->handle_object))
}
/* }}} */

/* {{{ proto bool Win\System\Event->pulse()
       pulses event - sets it to the on and then off state */
PHP_METHOD(WinSystemEvent, pulse)
{
	zend_error_handling error_handling;
	winsystem_event_object *event = (winsystem_event_object *)winsystem_event_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(PulseEvent(event->handle_object))
}
/* }}} */

/* {{{ proto bool Win\System\Event->canInherit()
      if a event was created with a "can inherit" flag, meaning child processes can grab it */
PHP_METHOD(WinSystemEvent, canInherit)
{
	zend_error_handling error_handling;
	winsystem_event_object *event = (winsystem_event_object *)winsystem_event_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(event->can_inherit)
}
/* }}} */

/* {{{ proto bool Win\System\Event->isAutoReset()
      if a event was created with the autoreset flag, meaning the event is automatically reset */
PHP_METHOD(WinSystemEvent, isAutoReset)
{
	zend_error_handling error_handling;
	winsystem_event_object *event = (winsystem_event_object *)winsystem_event_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(event->auto_reset)
}
/* }}} */

/* {{{ proto bool Win\System\Event->wait([int $milliseconds, bool $alertable])
        checks the current state of the specified mutex
		If the object's state is nonsignaled, the calling thread waits until the object is signaled
		or the time-out interval elapses. */
PHP_METHOD(WinSystemEvent, wait)
{
	DWORD ret;
	long milliseconds = INFINITE;
	winsystem_event_object *event = (winsystem_event_object *)winsystem_event_object_get(getThis() TSRMLS_CC);

	// TODO: parse parameters here and support EX

	if (event->handle_object == NULL) {
		//zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Missing internal data, remember to call parent::__construct in extended classes");
		return;
	}

	ret = WaitForSingleObject(event->handle_object, milliseconds);
	RETURN_LONG(ret);
}
/* }}} */

/* register event methods */
static zend_function_entry winsystem_event_functions[] = {
	PHP_ME(WinSystemEvent, __construct, WinSystemEvent___construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(WinSystemEvent, set, WinSystemEvent_set_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemEvent, reset, WinSystemEvent_reset_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemEvent, pulse, WinSystemEvent_pulse_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemEvent, getName, WinSystemEvent_getName_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemEvent, canInherit, WinSystemEvent_canInherit_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemEvent, isAutoReset, WinSystemEvent_isAutoReset_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemEvent, wait, WinSystemEvent_wait_args, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Event Object Magic LifeCycle Functions                                                    
------------------------------------------------------------------*/

/* cleans up the handle object */
static void winsystem_event_object_destructor(void *object TSRMLS_DC)
{
	winsystem_event_object *event_object = (winsystem_event_object *)object;

	zend_object_std_dtor(&event_object->std TSRMLS_CC);

	if(event_object->name) {
		efree(event_object->name);
	}
	if(event_object->handle_object != NULL){
		CloseHandle(event_object->handle_object);
	}
	
	efree(object);
}
/* }}} */

/* {{{ winsystem_event_object_new
       object that has an internal HANDLE stored */
static zend_object_value winsystem_event_object_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	winsystem_event_object *event_object;
	zval *tmp;

	event_object = emalloc(sizeof(winsystem_mutex_object));
	memset(&event_object->std, 0, sizeof(zend_object));
	event_object->handle_object = NULL;
	event_object->auto_reset = FALSE;
	event_object->can_inherit = FALSE;
	event_object->name = NULL;

	zend_object_std_init(&event_object->std, ce TSRMLS_CC);
	zend_hash_copy(event_object->std.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(event_object, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) winsystem_event_object_destructor, NULL TSRMLS_CC);
	event_object->handle = retval.handle;
	retval.handlers = zend_get_std_object_handlers();
	return retval;
}
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Event LifeCycle Functions                                                    
------------------------------------------------------------------*/
PHP_MINIT_FUNCTION(winsystem_event)
{
	zend_class_entry ce;
	
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Event", winsystem_event_functions);
	ce_winsystem_event = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(ce_winsystem_event TSRMLS_CC, 1, ce_winsystem_waitable);
	ce_winsystem_event->create_object = winsystem_event_object_new;

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
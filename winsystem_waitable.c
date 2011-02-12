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

#include "php_winsystem.h"
#include "zend_exceptions.h"
#include "implement_waitable.h"

zend_class_entry *ce_winsystem_waitable;
zend_class_entry *ce_winsystem_wait;
zend_class_entry *ce_winsystem_waitmask;

/* ----------------------------------------------------------------
  \Win\System\Wait Userland API
------------------------------------------------------------------*/
ZEND_BEGIN_ARG_INFO_EX(WinSystemWait_multiple_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, objects)
	ZEND_ARG_INFO(0, milliseconds)
	ZEND_ARG_INFO(0, wait_all)
	ZEND_ARG_INFO(0, alertable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(WinSystemWait_multipleMsg_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, objects)
	ZEND_ARG_INFO(0, milliseconds)
	ZEND_ARG_INFO(0, mask)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

/* {{{ proto int Win\System\Wait::multiple(array $objects[, int $milliseconds, bool $wait_all, bool $alertable])
         Waits until one or all of the specified objects are in the signaled state */
PHP_METHOD(WinSystemWait, multiple)
{
	DWORD ret;
	long milliseconds = INFINITE;
	zend_bool wait_all = FALSE;
	zend_bool alertable = FALSE;
	HashTable *objects = NULL;
	HashPosition pos;
	zval **data;
	DWORD i = 0;
	HANDLE *handles = NULL;
	winsystem_waitable_object *wait_object;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "H|lbb", &objects, &milliseconds, &wait_all, &alertable) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	handles = emalloc(sizeof(HANDLE) * MAXIMUM_WAIT_OBJECTS);

	for(zend_hash_internal_pointer_reset_ex(objects, &pos);
		zend_hash_get_current_data_ex(objects, (void**) &data, &pos) == SUCCESS;
		zend_hash_move_forward_ex(objects, &pos)) {

		if(Z_TYPE_PP(data) == IS_OBJECT &&
			instanceof_function(Z_OBJCE_PP(data), ce_winsystem_waitable TSRMLS_CC)) {
			wait_object = (winsystem_waitable_object *)zend_object_store_get_object(*data TSRMLS_CC);
			handles[i] = wait_object->handle;
			i++;
		} else {
			zend_throw_exception(ce_winsystem_argexception, "All objects must implement Waitable in order to be used", 0 TSRMLS_CC);
			efree(handles);
			return;
		}

		if (i == MAXIMUM_WAIT_OBJECTS)
			break;
	}

	if (i < 1) {
		zend_throw_exception(ce_winsystem_argexception, "No objects were registered to watch", 0 TSRMLS_CC);
		return;
	}

	ret = WaitForMultipleObjectsEx(i, handles, wait_all, milliseconds, alertable);
	if (ret == WAIT_FAILED) {
		winsystem_create_error(GetLastError(), ce_winsystem_exception TSRMLS_CC);
	}
	efree(handles);
	RETURN_LONG(ret);

}
/* }}} */

/* {{{ proto int Win\System\Wait::multipleMsg(array $objects[, int $milliseconds, int $wakemask, int $flags])
         Waits until one or all of the specified objects are in the signaled state */
PHP_METHOD(WinSystemWait, multipleMsg)
{
	DWORD ret;
	long milliseconds = INFINITE;
	long mask = QS_ALLEVENTS;
	long flags = 0;
	HashTable *objects = NULL;
	HashPosition pos;
	zval **data;
	DWORD i = 0;
	HANDLE *handles = NULL;
	winsystem_waitable_object *wait_object;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "H|lll", &objects, &milliseconds, &mask, &flags) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	handles = emalloc(sizeof(HANDLE) * MAXIMUM_WAIT_OBJECTS);

	for(zend_hash_internal_pointer_reset_ex(objects, &pos);
		zend_hash_get_current_data_ex(objects, (void**) &data, &pos) == SUCCESS;
		zend_hash_move_forward_ex(objects, &pos)) {

		if(Z_TYPE_PP(data) == IS_OBJECT &&
			instanceof_function(Z_OBJCE_PP(data), ce_winsystem_waitable TSRMLS_CC)) {
			wait_object = (winsystem_waitable_object *)zend_object_store_get_object(*data TSRMLS_CC);
			handles[i] = wait_object->handle;
			i++;
		} else {
			zend_throw_exception(ce_winsystem_argexception, "All objects must implement Waitable in order to be used", 0 TSRMLS_CC);
			efree(handles);
			return;
		}

		if (i == MAXIMUM_WAIT_OBJECTS)
			break;
	}

	if (i < 1) {
		zend_throw_exception(ce_winsystem_argexception, "No objects were registered to watch", 0 TSRMLS_CC);
		return;
	}

	ret = MsgWaitForMultipleObjectsEx(i, handles, milliseconds, mask, flags);
	if (ret == WAIT_FAILED) {
		winsystem_create_error(GetLastError(), ce_winsystem_exception TSRMLS_CC);
	}
	efree(handles);
	RETURN_LONG(ret);
}
/* }}} */

/* register methods */
static zend_function_entry winsystem_wait_functions[] = {
	PHP_ME(WinSystemWait, multiple, WinSystemWait_multiple_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemWait, multipleMsg, WinSystemWait_multipleMsg_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* ----------------------------------------------------------------
  \Win\System\Waitable Userland API
------------------------------------------------------------------*/

/* {{{ proto bool Win\System\Waitable->wait([int $milliseconds, bool $alertable])
		Waits until the specified object is in the signaled state, an I/O completion
		routine or asynchronous procedure call (APC) is queued to the thread, or the
		time-out interval elapses. */
PHP_METHOD(WinSystemWaitable, wait)
{
	DWORD ret;
	long milliseconds = INFINITE;
	zend_bool alertable = FALSE;
	zend_error_handling error_handling;
	winsystem_waitable_object *object = (winsystem_waitable_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lb", &milliseconds, &alertable) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	ret = WaitForSingleObjectEx(object->handle, milliseconds, alertable);
	if (ret == WAIT_FAILED) {
		winsystem_create_error(GetLastError(), ce_winsystem_exception TSRMLS_CC);
	}
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto bool Win\System\Waitable->waitMsg([int $milliseconds, int $mask, bool $alertable])
        Waits until the specified object is in the signaled state */
PHP_METHOD(WinSystemWaitable, waitMsg)
{
	DWORD ret;
	long milliseconds = INFINITE;
	long mask = QS_ALLEVENTS;
	zend_bool alertable = FALSE;
	long flags = 0;
	HANDLE *handles = NULL;
	zend_error_handling error_handling;
	winsystem_waitable_object *object = (winsystem_waitable_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|llb", &milliseconds, &mask, &alertable) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	handles = emalloc(sizeof(HANDLE) * 1);
	handles[0] = object->handle;

	if (alertable) {
		flags = MWMO_ALERTABLE;
	}

	ret = MsgWaitForMultipleObjectsEx(1, handles, milliseconds, mask, flags);
	if (ret == WAIT_FAILED) {
		winsystem_create_error(GetLastError(), ce_winsystem_exception TSRMLS_CC);
	}
	efree(handles);

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto bool Win\System\Waitable->signalAndWait(Waitable $object[, int $milliseconds, bool $alertable])
        Signals the provided object and waits */
PHP_METHOD(WinSystemWaitable, signalAndWait)
{
	DWORD ret;
	long milliseconds = INFINITE;
	zend_bool alertable = FALSE;
	zval *signal_zval = NULL;
	winsystem_waitable_object *signal;
	zend_error_handling error_handling;
	winsystem_waitable_object *object = (winsystem_waitable_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "O|lb", &signal_zval, ce_winsystem_waitable, &milliseconds, &alertable) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	signal = (winsystem_waitable_object *)zend_object_store_get_object(signal_zval TSRMLS_CC);

	ret = SignalObjectAndWait(signal->handle, object->handle, milliseconds, alertable);
	if (ret == WAIT_FAILED) {
		winsystem_create_error(GetLastError(), ce_winsystem_exception TSRMLS_CC);
	}
	RETURN_LONG(ret);
}
/* }}} */

/* register methods */
static zend_function_entry winsystem_waitable_functions[] = {
	PHP_ABSTRACT_ME(WinSystemWaitable, wait, WinSystemWaitable_wait_args)
	PHP_ABSTRACT_ME(WinSystemWaitable, waitMsg, WinSystemWaitable_waitMsg_args)
	PHP_ABSTRACT_ME(WinSystemWaitable, signalAndWait, WinSystemWaitable_signalAndWait_args)
	{NULL, NULL, NULL}
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Waitable LifeCycle Functions
------------------------------------------------------------------*/
PHP_MINIT_FUNCTION(winsystem_waitable)
{
	zend_class_entry ce, wait_ce, mask_ce;
	
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Waitable", winsystem_waitable_functions);
	ce_winsystem_waitable = zend_register_internal_class(&ce TSRMLS_CC);
	ce_winsystem_waitable->ce_flags |= ZEND_ACC_INTERFACE;

	INIT_NS_CLASS_ENTRY(wait_ce, PHP_WINSYSTEM_NS, "Wait", winsystem_wait_functions);
	ce_winsystem_wait = zend_register_internal_class(&wait_ce TSRMLS_CC);
	ce_winsystem_wait->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS | ZEND_ACC_FINAL_CLASS;

	zend_declare_class_constant_long(ce_winsystem_wait, "INFINITE", sizeof("INFINITE")-1, INFINITE TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_wait, "ABANDONED", sizeof("ABANDONED")-1, WAIT_ABANDONED TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_wait, "IO_COMPLETION", sizeof("IO_COMPLETION")-1, WAIT_IO_COMPLETION TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_wait, "OBJECTS", sizeof("OBJECTS")-1, WAIT_OBJECT_0 TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_wait, "TIMEOUT", sizeof("TIMEOUT")-1, WAIT_TIMEOUT TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_wait, "FAILED", sizeof("FAILED")-1, WAIT_FAILED TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_wait, "ALERTABLE", sizeof("ALERTABLE")-1, MWMO_ALERTABLE TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_wait, "INPUTAVAILABLE", sizeof("INPUTAVAILABLE")-1, MWMO_INPUTAVAILABLE TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_wait, "WAITALL", sizeof("WAITALL")-1, MWMO_WAITALL TSRMLS_CC);

	INIT_NS_CLASS_ENTRY(mask_ce, PHP_WINSYSTEM_NS, "WaitMask", NULL);
	ce_winsystem_waitmask = zend_register_internal_class(&mask_ce TSRMLS_CC);
	ce_winsystem_waitmask->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS | ZEND_ACC_FINAL_CLASS;

	zend_declare_class_constant_long(ce_winsystem_waitmask, "ALLEVENTS", sizeof("ALLEVENTS")-1, QS_ALLEVENTS TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_waitmask, "ALLINPUT", sizeof("ALLINPUT")-1, QS_ALLINPUT TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_waitmask, "ALLPOSTMESSAGE", sizeof("ALLPOSTMESSAGE")-1, QS_ALLPOSTMESSAGE TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_waitmask, "HOTKEY", sizeof("HOTKEY")-1, QS_HOTKEY TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_waitmask, "INPUT", sizeof("INPUT")-1, QS_INPUT TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_waitmask, "KEY", sizeof("KEY")-1, QS_KEY TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_waitmask, "MOUSE", sizeof("MOUSE")-1, QS_MOUSE TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_waitmask, "MOUSEBUTTON", sizeof("MOUSEBUTTON")-1, QS_MOUSEBUTTON TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_waitmask, "MOUSEMOVE", sizeof("MOUSEMOVE")-1, QS_MOUSEMOVE TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_waitmask, "PAINT", sizeof("PAINT")-1, QS_PAINT TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_waitmask, "POSTMESSAGE", sizeof("POSTMESSAGE")-1, QS_POSTMESSAGE TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_waitmask, "RAWINPUT", sizeof("RAWINPUT")-1, QS_RAWINPUT TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_waitmask, "SENDMESSAGE", sizeof("SENDMESSAGE")-1, QS_SENDMESSAGE TSRMLS_CC);
	zend_declare_class_constant_long(ce_winsystem_waitmask, "TIMER", sizeof("TIMER")-1, QS_TIMER TSRMLS_CC);

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
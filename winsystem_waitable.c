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

#include "php_winsystem.h"

/* All the classes in this file */
zend_class_entry *ce_winsystem_waitable;

/* ----------------------------------------------------------------
  \Win\System\Waitable Userland API                                                    
------------------------------------------------------------------*/
ZEND_BEGIN_ARG_INFO(WinSystemWaitable_wait_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

/* {{{ proto bool Win\System\Waitable->wait([int $milliseconds, bool $alertable])
        checks the current state of the specified mutex
		If the object's state is nonsignaled, the calling thread waits until the object is signaled
		or the time-out interval elapses. */
PHP_METHOD(WinSystemWaitable, wait)
{
	DWORD ret;
	long milliseconds = INFINITE;
	winsystem_mutex_object *object = (winsystem_mutex_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	// TODO: parse parameters here and support EX

	ret = WaitForSingleObject(object->handle, milliseconds);
	object->is_owned = TRUE;
	RETURN_LONG(ret);
}
/* }}} */

/* register methods */
static zend_function_entry winsystem_waitable_functions[] = {
	PHP_ME(WinSystemWaitable, wait, WinSystemWaitable_wait_args, ZEND_ACC_PUBLIC|ZEND_ACC_ABSTRACT)
	{NULL, NULL, NULL}
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Waitable LifeCycle Functions                                                    
------------------------------------------------------------------*/
PHP_MINIT_FUNCTION(winsystem_waitable)
{
	zend_class_entry ce;
	
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Waitable", winsystem_waitable_functions);
	ce_winsystem_waitable = zend_register_internal_class(&ce TSRMLS_CC);
	ce_winsystem_waitable->ce_flags |= ZEND_ACC_INTERFACE;

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
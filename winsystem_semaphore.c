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
zend_class_entry *ce_winsystem_semaphore;

static inline winsystem_semaphore_object* winsystem_semaphore_object_get(zval *zobj TSRMLS_DC)
{
    winsystem_semaphore_object *pobj = zend_object_store_get_object(zobj TSRMLS_CC);
    if (pobj->handle_object == NULL) {
		php_error(E_ERROR, "Internal semaphore handle missing in %s class, you must call parent::__construct in extended classes", Z_OBJCE_P(zobj)->name);
		return NULL;
    }
    return pobj;
}

/* ----------------------------------------------------------------
  Win\System\Semaphore Userland API                                                    
------------------------------------------------------------------*/
ZEND_BEGIN_ARG_INFO_EX(WinSystemSemaphore___construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, start)
    ZEND_ARG_INFO(0, maxcount)
	ZEND_ARG_INFO(0, inherit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemSemaphore_release_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemSemaphore_getCount_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemSemaphore_getMaxCount_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemSemaphore_getName_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemSemaphore_canInherit_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemSemaphore_wait_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

/* {{{ proto bool Win\System\Semphore->__construct()
       creates a new semaphore, can be named, minimum max count is 1,
	   count not not be greater then max count */
PHP_METHOD(WinSystemSemaphore, __construct)
{
	SECURITY_ATTRIBUTES semaphore_attributes;
	zend_bool inherit = TRUE;
	char * name = NULL;
	int name_length;
	long count = 0, max_count = 1;
	HANDLE semaphore_handle;
	zend_error_handling error_handling;
	winsystem_semaphore_object *semaphore = (winsystem_semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!llb", &name, &name_length, &count, &max_count, &inherit) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (count < 0 || count > max_count) {
		zend_throw_exception(ce_winsystem_exception, "Count must be less than the max_count and 0 or greater", 0 TSRMLS_CC);
		return;
	}

	if (max_count < 1) {
		zend_throw_exception(ce_winsystem_exception, "Max count must be at least 1", 0 TSRMLS_CC);
		return;
	}

	semaphore_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	semaphore_attributes.lpSecurityDescriptor = NULL;
	semaphore_attributes.bInheritHandle = inherit;

	semaphore_handle = CreateSemaphore(&semaphore_attributes, count, max_count, name);

	/* we couldn't open it, try to open the semaphore with only synchronize rights */
	if (semaphore_handle == NULL) {
		semaphore_handle = OpenSemaphore(SYNCHRONIZE, inherit, name);
	}

	/* semaphore is STILL null, we couldn't create the semaphore, fail */
	if (semaphore_handle == NULL) {
		zend_throw_exception(ce_winsystem_exception, "Could not create or open the requested semaphore", 0 TSRMLS_CC);
		return;
	}

	semaphore->handle_object = semaphore_handle;
	semaphore->count = count;
	semaphore->max_count = max_count;
	semaphore->can_inherit = inherit;
	if(name) {
		semaphore->name = estrdup(name);
	}
}
/* }}} */

/* {{{ proto bool Win\System\Semaphore->getName()
       returns the current name of the semaphore or null if it's unnamed */
PHP_METHOD(WinSystemSemaphore, getName)
{
	zend_error_handling error_handling;
	winsystem_semaphore_object *semaphore = (winsystem_semaphore_object*)winsystem_semaphore_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (semaphore->name) {
		RETURN_STRING(semaphore->name, 1)
	}
	RETURN_NULL()
}
/* }}} */

/* {{{ proto bool Win\System\Semaphore->release()
       releases a semaphore, increases the semaphore count, if count is specified, it is increased by that
	   specific number */
PHP_METHOD(WinSystemSemaphore, release)
{
	BOOL worked;
	int error;
	long count = 1, prev_count;
	zend_error_handling error_handling;
	winsystem_semaphore_object *semaphore = (winsystem_semaphore_object*)winsystem_semaphore_object_get(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &count) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	worked = ReleaseSemaphore(semaphore->handle_object, count, &prev_count);
	if (worked == 0) {
		error = GetLastError();
		if (error) {
			winsystem_create_error(error, ce_winsystem_exception TSRMLS_CC);
		}
		RETURN_FALSE
	}
	semaphore->count = MIN(prev_count + count, semaphore->max_count);

	RETURN_TRUE
}
/* }}} */

/* {{{ proto bool Win\System\Semaphore->getCount()
      get the current semaphore count */
PHP_METHOD(WinSystemSemaphore, getCount)
{
	zend_error_handling error_handling;
	winsystem_semaphore_object *semaphore = (winsystem_semaphore_object*)winsystem_semaphore_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_LONG(semaphore->count)
}
/* }}} */

/* {{{ proto bool Win\System\Semaphore->getMaxCount()
      get the maximum allowed semaphore count */
PHP_METHOD(WinSystemSemaphore, getMaxCount)
{
	zend_error_handling error_handling;
	winsystem_semaphore_object *semaphore = (winsystem_semaphore_object*)winsystem_semaphore_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_LONG(semaphore->max_count)
}
/* }}} */

/* {{{ proto bool Win\System\Semaphore->canInherit()
      if a semaphore was created with a "can inherit" flag, meaning child processes can grab it */
PHP_METHOD(WinSystemSemaphore, canInherit)
{
	zend_error_handling error_handling;
	winsystem_semaphore_object *semaphore = (winsystem_semaphore_object*)winsystem_semaphore_object_get(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(semaphore->can_inherit)
}
/* }}} */

/* {{{ proto bool Win\System\Semaphore->wait([int $milliseconds, bool $alertable])
        if the semaphore count is greater than one, decrease the semaphore count by 1 */
PHP_METHOD(WinSystemSemaphore, wait)
{
	DWORD ret;
	long milliseconds = INFINITE;
	winsystem_semaphore_object *semaphore = (winsystem_semaphore_object*)winsystem_semaphore_object_get(getThis() TSRMLS_CC);
	
	// TODO: parse parameters here and support EX

	if (semaphore->handle_object == NULL) {
		//zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Missing internal data, remember to call parent::__construct in extended classes");
		return;
	}

	ret = WaitForSingleObject(semaphore->handle_object, milliseconds);
	semaphore->count--;
	RETURN_LONG(ret);
}
/* }}} */

/* register semaphore methods */
static zend_function_entry winsystem_semaphore_functions[] = {
	PHP_ME(WinSystemSemaphore, __construct, WinSystemSemaphore___construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(WinSystemSemaphore, release, WinSystemSemaphore_release_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemSemaphore, getCount, WinSystemSemaphore_getCount_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemSemaphore, getMaxCount, WinSystemSemaphore_getMaxCount_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemSemaphore, getName, WinSystemSemaphore_getName_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemSemaphore, canInherit, WinSystemSemaphore_canInherit_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemSemaphore, wait, WinSystemSemaphore_wait_args, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Semaphore Object Magic LifeCycle Functions                                                    
------------------------------------------------------------------*/

/* cleans up the handle object */
static void winsystem_semaphore_object_destructor(void *object TSRMLS_DC)
{
	winsystem_semaphore_object *semaphore_object = (winsystem_semaphore_object *)object;

	zend_object_std_dtor(&semaphore_object->std TSRMLS_CC);

	if(semaphore_object->name) {
		efree(semaphore_object->name);
	}
	if(semaphore_object->handle_object != NULL){
		CloseHandle(semaphore_object->handle_object);
	}
	
	efree(object);
}
/* }}} */

/* {{{ winsystem_semaphore_object_new
       object that has an internal HANDLE stored */
static zend_object_value winsystem_semaphore_object_new(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	winsystem_semaphore_object *semaphore_object;
	zval *tmp;

	semaphore_object = emalloc(sizeof(winsystem_semaphore_object));
	memset(&semaphore_object->std, 0, sizeof(zend_object));
	semaphore_object->handle_object = NULL;
	semaphore_object->count = 0;
	semaphore_object->max_count = 0;
	semaphore_object->can_inherit = FALSE;
	semaphore_object->name = NULL;

	zend_object_std_init(&semaphore_object->std, ce TSRMLS_CC);
	zend_hash_copy(semaphore_object->std.properties, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(semaphore_object, (zend_objects_store_dtor_t)zend_objects_destroy_object, (zend_objects_free_object_storage_t) winsystem_semaphore_object_destructor, NULL TSRMLS_CC);
	semaphore_object->handle = retval.handle;
	retval.handlers = zend_get_std_object_handlers();
	return retval;
}
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Semaphore LifeCycle Functions                                                    
------------------------------------------------------------------*/
PHP_MINIT_FUNCTION(winsystem_semaphore)
{
	zend_class_entry ce;
	
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Semaphore", winsystem_semaphore_functions);
	ce_winsystem_semaphore = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(ce_winsystem_semaphore TSRMLS_CC, 1, ce_winsystem_waitable);
	ce_winsystem_semaphore->create_object = winsystem_semaphore_object_new;

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
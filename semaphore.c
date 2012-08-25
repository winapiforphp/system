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
#include "waitable.h"

zend_class_entry *ce_winsystem_semaphore;
static zend_object_handlers winsystem_semaphore_object_handlers;
static zend_function winsystem_semaphore_constructor_wrapper;

/* ----------------------------------------------------------------
  Win\System\Semaphore Userland API
------------------------------------------------------------------*/
ZEND_BEGIN_ARG_INFO_EX(WinSystemSemaphore___construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, start)
	ZEND_ARG_INFO(0, maxcount)
	ZEND_ARG_INFO(0, inherit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(WinSystemSemaphore_open_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, process_inherit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemSemaphore_release_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemSemaphore_getMaxCount_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemSemaphore_getName_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemSemaphore_canInherit_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

/* {{{ proto object Win\System\Semphore->__construct([string|Unicode name[, start[, maxcount[, inherit]]]])
       creates a new semaphore, can be named, minimum max count is 1,
	   count not not be greater then max count */
PHP_METHOD(WinSystemSemaphore, __construct)
{
	/* Used for regular string */
	char * name = NULL;
	int name_length;

	/* Used for wchar string */
	zval *unicode = NULL;
	winsystem_unicode_object *unicode_object = NULL;
	int use_unicode = 0;

	SECURITY_ATTRIBUTES semaphore_attributes;
	zend_bool inherit = TRUE;
	long count = 0, max_count = 1;
	HANDLE semaphore_handle;
	zend_error_handling error_handling;
	winsystem_semaphore_object *semaphore = (winsystem_semaphore_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);

	/* version one, use unicode */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|llb", &unicode, ce_winsystem_unicode, &count, &max_count, &inherit) != FAILURE) {
		use_unicode = 1;
		unicode_object = (winsystem_unicode_object *)zend_object_store_get_object(unicode TSRMLS_CC);
	} else if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!llb", &name, &name_length, &count, &max_count, &inherit) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (count < 0 || count > max_count) {
		zend_throw_exception(ce_winsystem_argexception, "Count must be less than the max_count and 0 or greater", 0 TSRMLS_CC);
		return;
	}

	if (max_count < 1) {
		zend_throw_exception(ce_winsystem_argexception, "Max count must be at least 1", 0 TSRMLS_CC);
		return;
	}

	semaphore_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	semaphore_attributes.lpSecurityDescriptor = NULL;
	semaphore_attributes.bInheritHandle = inherit;

	if (use_unicode) {
		semaphore_handle = CreateSemaphoreW(&semaphore_attributes, count, max_count, unicode_object->unicode_string);
	} else {
		semaphore_handle = CreateSemaphoreA(&semaphore_attributes, count, max_count, name);
	}

	/* semaphore is STILL null, we couldn't create the semaphore, fail  */
	if (semaphore_handle == NULL) {
		DWORD error_num = GetLastError();

		if (error_num == ERROR_INVALID_HANDLE) {
			zend_throw_exception(ce_winsystem_exception, "Name is already in use for waitable object", error_num TSRMLS_CC);
		} else {
			winsystem_create_error(error_num, ce_winsystem_exception TSRMLS_CC);
		}
		return;
	}

	semaphore->handle = semaphore_handle;
	semaphore->max_count = max_count;
	semaphore->can_inherit = inherit;
	semaphore->is_constructed = TRUE;
	if(name) {
		semaphore->name.string = estrdup(name);
	} else if (use_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(unicode);
		semaphore->name.unicode_object = unicode;
		semaphore->is_unicode = TRUE;
	}
}
/* }}} */

/* {{{ proto object Win\System\WinSystemSemaphore::open(string|Unicode name[, inherit])
       attempts to open an existing event */
PHP_METHOD(WinSystemSemaphore, open)
{
	/* Used for regular string */
	char * name = NULL;
	int name_length;

	/* Used for wchar string */
	zval *unicode = NULL;
	winsystem_unicode_object *unicode_object = NULL;
	int use_unicode = 0;

	/* Variables for a and w versions */
	zend_bool inherit = TRUE;
	HANDLE semaphore_handle;
	winsystem_semaphore_object *semaphore;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);

	/* version one, use unicode */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|b", &unicode, ce_winsystem_unicode, &inherit) != FAILURE) {
		use_unicode = 1;
		unicode_object = (winsystem_unicode_object *)zend_object_store_get_object(unicode TSRMLS_CC);
	} else if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &name, &name_length, &inherit) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	object_init_ex(return_value, ce_winsystem_semaphore);
	semaphore = (winsystem_semaphore_object *)zend_object_store_get_object(return_value TSRMLS_CC);

	if (use_unicode) {
		semaphore_handle = OpenSemaphoreW(SYNCHRONIZE, inherit, unicode_object->unicode_string);
	} else {
		semaphore_handle = OpenSemaphoreA(SYNCHRONIZE, inherit, name);
	}

	if (semaphore_handle == NULL) {
		DWORD error_num = GetLastError();

		if (error_num == ERROR_FILE_NOT_FOUND) {
			zend_throw_exception(ce_winsystem_exception, "Semaphore was not found and could not be opened", error_num TSRMLS_CC);
		} else {
			winsystem_create_error(error_num, ce_winsystem_exception TSRMLS_CC);
		}
		return;
	}

	semaphore->handle = semaphore_handle;
	semaphore->max_count = -1;
	semaphore->can_inherit = inherit;
	semaphore->is_constructed = TRUE;
	if(name) {
		semaphore->name.string = estrdup(name);
	} else if (use_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(unicode);
		semaphore->name.unicode_object = unicode;
		semaphore->is_unicode = TRUE;
	}
}
/* }}} */

/* {{{ proto bool Win\System\Semaphore->getName()
       returns the current name of the semaphore or null if it's unnamed */
PHP_METHOD(WinSystemSemaphore, getName)
{
	zend_error_handling error_handling;
	winsystem_semaphore_object *semaphore = (winsystem_semaphore_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (semaphore->is_unicode) {
		RETURN_ZVAL(semaphore->name.unicode_object, 1, 0);
	} else if (semaphore->name.string) {
		RETURN_STRING(semaphore->name.string, 1)
	}
	RETURN_NULL()
}
/* }}} */

/* {{{ proto int Win\System\Semaphore->release()
       releases a semaphore, increases the semaphore count, if count is specified, it is increased by that
	   specific number */
PHP_METHOD(WinSystemSemaphore, release)
{
	BOOL worked;
	int error;
	long count = 1, prev_count;
	zend_error_handling error_handling;
	winsystem_semaphore_object *semaphore = (winsystem_semaphore_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &count) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	worked = ReleaseSemaphore(semaphore->handle, count, &prev_count);
	if (worked == 0) {
		error = GetLastError();
		if (error) {
			winsystem_create_error(error, ce_winsystem_exception TSRMLS_CC);
		}
		RETURN_FALSE
	}

	RETURN_LONG(prev_count);
}
/* }}} */

/* {{{ proto bool Win\System\Semaphore->getMaxCount()
      get the maximum allowed semaphore count */
PHP_METHOD(WinSystemSemaphore, getMaxCount)
{
	zend_error_handling error_handling;
	winsystem_semaphore_object *semaphore = (winsystem_semaphore_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (semaphore->max_count < 0) {
		zend_throw_exception(ce_winsystem_exception, "Max semaphore count unknown", 0 TSRMLS_CC);
	}

	RETURN_LONG(semaphore->max_count)
}
/* }}} */

/* {{{ proto bool Win\System\Semaphore->canInherit()
      if a semaphore was created with a "can inherit" flag, meaning child processes can grab it */
PHP_METHOD(WinSystemSemaphore, canInherit)
{
	zend_error_handling error_handling;
	winsystem_semaphore_object *semaphore = (winsystem_semaphore_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(semaphore->can_inherit)
}
/* }}} */

/* register semaphore methods */
static zend_function_entry winsystem_semaphore_functions[] = {
	PHP_ME(WinSystemSemaphore, __construct, WinSystemSemaphore___construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(WinSystemSemaphore, open, WinSystemSemaphore_open_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemSemaphore, release, WinSystemSemaphore_release_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemSemaphore, getMaxCount, WinSystemSemaphore_getMaxCount_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemSemaphore, getName, WinSystemSemaphore_getName_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemSemaphore, canInherit, WinSystemSemaphore_canInherit_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, wait, WinSystemWaitable_wait_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, waitMsg, WinSystemWaitable_waitMsg_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, signalAndWait, WinSystemWaitable_signalAndWait_args, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Semaphore Object Magic LifeCycle Functions
------------------------------------------------------------------*/

/* {{{ winsystem_semaphore_construction_wrapper
       wraps around the constructor to make sure parent::__construct is always called  */
static void winsystem_semaphore_construction_wrapper(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *this = getThis();
	winsystem_semaphore_object *tobj;
	zend_class_entry *this_ce;
	zend_function *zf;
	zend_fcall_info fci = {0};
	zend_fcall_info_cache fci_cache = {0};
	zval *retval_ptr = NULL;
	unsigned i;
 
	tobj = zend_object_store_get_object(this TSRMLS_CC);
	zf = zend_get_std_object_handlers()->get_constructor(this TSRMLS_CC);
	this_ce = Z_OBJCE_P(this);
 
	fci.size = sizeof(fci);
	fci.function_table = &this_ce->function_table;
	fci.object_ptr = this;
	/* fci.function_name = ; not necessary to bother */
	fci.retval_ptr_ptr = &retval_ptr;
	fci.param_count = ZEND_NUM_ARGS();
	fci.params = emalloc(fci.param_count * sizeof *fci.params);
	/* Or use _zend_get_parameters_array_ex instead of loop: */
	for (i = 0; i < fci.param_count; i++) {
		fci.params[i] = (zval **) (zend_vm_stack_top(TSRMLS_C) - 1 -
			(fci.param_count - i));
	}
	fci.object_ptr = this;
	fci.no_separation = 0;
 
	fci_cache.initialized = 1;
	fci_cache.called_scope = EG(current_execute_data)->called_scope;
	fci_cache.calling_scope = EG(current_execute_data)->current_scope;
	fci_cache.function_handler = zf;
	fci_cache.object_ptr = this;
 
	zend_call_function(&fci, &fci_cache TSRMLS_CC);
	if (!EG(exception) && tobj->is_constructed == 0)
		zend_throw_exception_ex(ce_winsystem_exception, 0 TSRMLS_CC,
			"parent::__construct() must be called in %s::__construct()", this_ce->name);
	efree(fci.params);
	zval_ptr_dtor(&retval_ptr);
}
/* }}} */

/* {{{ winsystem_semaphore_get_constructor
       gets the constructor for the class  */
static zend_function *winsystem_semaphore_get_constructor(zval *object TSRMLS_DC)
{
	/* Could always return constr_wrapper_fun, but it's uncessary to call the
	 * wrapper if instantiating the superclass */
	if (Z_OBJCE_P(object) == ce_winsystem_semaphore)
		return zend_get_std_object_handlers()->
			get_constructor(object TSRMLS_CC);
	else
		return &winsystem_semaphore_constructor_wrapper;
}
/* }}} */

/* {{{ winsystem_semaphore_object_free
       frees up the semaphore handle underneath as well as any stored
       unicode object or string name */
static void winsystem_semaphore_object_free(void *object TSRMLS_DC)
{
	winsystem_semaphore_object *semaphore_object = (winsystem_semaphore_object *)object;

	zend_object_std_dtor(&semaphore_object->std TSRMLS_CC);

	if (semaphore_object->is_unicode) {
		Z_DELREF_P(semaphore_object->name.unicode_object);
	} else if (semaphore_object->name.string) {
		efree(semaphore_object->name.string);
	}

	if(semaphore_object->handle != NULL){
		CloseHandle(semaphore_object->handle);
	}
	
	efree(semaphore_object);
}
/* }}} */

/* {{{ winsystem_semaphore_object_create
       object that has an internal HANDLE stored  */
static zend_object_value winsystem_semaphore_object_create(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value          retval;
	winsystem_semaphore_object *semaphore_object;

	semaphore_object = ecalloc(1, sizeof(winsystem_semaphore_object));
	zend_object_std_init(&semaphore_object->std, ce TSRMLS_CC);
	semaphore_object->handle = NULL;
	semaphore_object->max_count = -1;
	semaphore_object->can_inherit = FALSE;
	semaphore_object->is_unicode = FALSE;
 
	object_properties_init(&semaphore_object->std, ce);
 
	retval.handle = zend_objects_store_put(semaphore_object,
		(zend_objects_store_dtor_t) zend_objects_destroy_object,
		(zend_objects_free_object_storage_t) winsystem_semaphore_object_free,
		NULL TSRMLS_CC);
	retval.handlers = &winsystem_semaphore_object_handlers;
	return retval;
}
/* }}} */

/* {{{ winsystem_semaphore_object_clone
       duplicates the internal handle object, string dups a string
	   or add refs an object */
static zend_object_value winsystem_semaphore_object_clone(zval *this_ptr TSRMLS_DC)
{
	zend_object_value          retval;
	winsystem_semaphore_object     *new_semaphore_object;
	winsystem_semaphore_object     *old_semaphore_object = (winsystem_semaphore_object *) zend_object_store_get_object(this_ptr TSRMLS_CC);
 
	new_semaphore_object = ecalloc(1, sizeof(winsystem_semaphore_object));
	zend_object_std_init(&new_semaphore_object->std, old_semaphore_object->std.ce TSRMLS_CC);
	DuplicateHandle(GetCurrentProcess(), 
					old_semaphore_object->handle, 
					GetCurrentProcess(),
					&new_semaphore_object->handle, 
					0, /* Ignored, we're using same_access */
					old_semaphore_object->can_inherit,
					DUPLICATE_SAME_ACCESS);

	new_semaphore_object->can_inherit = old_semaphore_object->can_inherit;
	new_semaphore_object->max_count = old_semaphore_object->max_count;
	new_semaphore_object->can_inherit = old_semaphore_object->can_inherit;
	new_semaphore_object->is_constructed = TRUE;

	if (old_semaphore_object->is_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(old_semaphore_object->name.unicode_object);
		new_semaphore_object->name.unicode_object = old_semaphore_object->name.unicode_object;
		new_semaphore_object->is_unicode = TRUE;
	} else if (old_semaphore_object->name.string) {
		new_semaphore_object->name.string = estrdup(old_semaphore_object->name.string);
	}

	object_properties_init(&new_semaphore_object->std, old_semaphore_object->std.ce);

	retval.handle = zend_objects_store_put(new_semaphore_object,
		(zend_objects_store_dtor_t) zend_objects_destroy_object,
		(zend_objects_free_object_storage_t) winsystem_semaphore_object_free,
		NULL TSRMLS_CC);
	retval.handlers = &winsystem_semaphore_object_handlers;

	zend_objects_clone_members(&new_semaphore_object->std, retval, zend_objects_get_address(this_ptr TSRMLS_CC), Z_OBJ_HANDLE_P(this_ptr) TSRMLS_CC);
	return retval; 
}
/* }}} */

/* {{{ winsystem_semaphore_get_debug_info
       pretty var_dumps  */
static HashTable *winsystem_semaphore_get_debug_info(zval *obj, int *is_temp TSRMLS_DC)
{
	winsystem_semaphore_object *object = (winsystem_semaphore_object *) zend_object_store_get_object(obj TSRMLS_CC);

	HashTable *retval, *std_props;
	zval *tmp;
	char *can_inherit, *name, *max_count;
	int can_inherit_len, name_len, max_count_len;

	ALLOC_HASHTABLE(retval);
	zend_hash_init(retval, 3, NULL, ZVAL_PTR_DTOR, 0);

	std_props = zend_std_get_properties(obj TSRMLS_CC);
	zend_hash_copy(retval, std_props, (copy_ctor_func_t)zval_add_ref, NULL, sizeof(zval*));

	zend_mangle_property_name(&max_count, &max_count_len, 
		object->std.ce->name, object->std.ce->name_length, "maxCount", sizeof("maxCount") -1, 0);
	MAKE_STD_ZVAL(tmp);
	ZVAL_LONG(tmp, object->max_count);
	zend_hash_update(retval, max_count, max_count_len + 1, &tmp, sizeof(zval*), NULL);
	efree(max_count);

	zend_mangle_property_name(&can_inherit, &can_inherit_len, 
		object->std.ce->name, object->std.ce->name_length, "canInherit", sizeof("canInherit") -1, 0);
	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, object->can_inherit);
	zend_hash_update(retval, can_inherit, can_inherit_len + 1, &tmp, sizeof(zval*), NULL);
	efree(can_inherit);

	zend_mangle_property_name(&name, &name_len, 
		object->std.ce->name, object->std.ce->name_length, "name", sizeof("name") -1, 0);
	if (object->is_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(object->name.unicode_object);
		zend_hash_update(retval, name, name_len + 1, &object->name.unicode_object, sizeof(zval*), NULL);
	} else {
		MAKE_STD_ZVAL(tmp);
		ZVAL_STRING(tmp, object->name.string, 1);
		zend_hash_update(retval, name, name_len + 1, &tmp, sizeof(zval*), NULL);
	}
	efree(name);

	*is_temp = 1;
	return retval; 
}
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Semaphore LifeCycle Functions
------------------------------------------------------------------*/
PHP_MINIT_FUNCTION(winsystem_semaphore)
{
	zend_class_entry ce;

	memcpy(&winsystem_semaphore_object_handlers, zend_get_std_object_handlers(),
		sizeof winsystem_semaphore_object_handlers);
	winsystem_semaphore_object_handlers.get_constructor = winsystem_semaphore_get_constructor;
	winsystem_semaphore_object_handlers.clone_obj = winsystem_semaphore_object_clone;
	winsystem_semaphore_object_handlers.get_debug_info = winsystem_semaphore_get_debug_info;

	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Semaphore", winsystem_semaphore_functions);
	ce_winsystem_semaphore = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(ce_winsystem_semaphore TSRMLS_CC, 1, ce_winsystem_waitable);
	ce_winsystem_semaphore->create_object = winsystem_semaphore_object_create;

	winsystem_semaphore_constructor_wrapper.type = ZEND_INTERNAL_FUNCTION;
	winsystem_semaphore_constructor_wrapper.common.function_name = "internal_construction_wrapper";
	winsystem_semaphore_constructor_wrapper.common.scope = ce_winsystem_semaphore;
	winsystem_semaphore_constructor_wrapper.common.fn_flags = ZEND_ACC_PROTECTED;
	winsystem_semaphore_constructor_wrapper.common.prototype = NULL;
	winsystem_semaphore_constructor_wrapper.common.required_num_args = 0;
	winsystem_semaphore_constructor_wrapper.common.arg_info = NULL;
	winsystem_semaphore_constructor_wrapper.internal_function.handler = winsystem_semaphore_construction_wrapper;
	winsystem_semaphore_constructor_wrapper.internal_function.module = EG(current_module);

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
/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Elizabeth Smith <auroraeosrose@gmail.com>                    |
  +----------------------------------------------------------------------+
*/

#include "php_winsystem.h"
#include "zend_exceptions.h"
#include "waitable.h"

ZEND_DECLARE_MODULE_GLOBALS(winsystem);

zend_class_entry *ce_winsystem_mutex;
static zend_object_handlers winsystem_mutex_object_handlers;
static zend_function winsystem_mutex_constructor_wrapper;

struct _winsystem_mutex_object {
	zend_object    std;
	zend_bool      is_constructed;
	HANDLE         handle;
	BOOL           can_inherit;
	zend_bool      is_unicode;
	winsystem_name name;
};

/* ----------------------------------------------------------------
  Win\System\Mutex Userland API
------------------------------------------------------------------*/
ZEND_BEGIN_ARG_INFO_EX(WinSystemMutex___construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, own)
	ZEND_ARG_INFO(0, process_inherit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(WinSystemMutex_open_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, process_inherit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemMutex_release_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemMutex_getName_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemMutex_canInherit_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()


/* {{{ proto object Win\System\Mutex->__construct([string|Unicode name[, own[, inherit]]])
       creates a new mutex, optionally sets the owner to the thread that called it
       if the mutex cannot be created, will attempt to open it instead
       if ownership is requested, and the mutex cannot be owned this will throw an exception */
PHP_METHOD(WinSystemMutex, __construct)
{
	/* Used for regular string */
	char * name = NULL;
	int name_length;

	/* Used for wchar string */
	zval *unicode = NULL;
	int use_unicode = 0;

	/* Variables for a and w versions */
	SECURITY_ATTRIBUTES mutex_attributes;
	zend_bool inherit = TRUE;
	zend_bool own = FALSE;
	HANDLE mutex_handle;
	DWORD error_num;
	winsystem_mutex_object *mutex = (winsystem_mutex_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	PHP_WINSYSTEM_EXCEPTIONS
	/* version one, use unicode */
	if (FAILURE != zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|bb", &unicode, ce_winsystem_unicode, &own, &inherit)) {
		use_unicode = 1;
	} else if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!bb", &name, &name_length, &own, &inherit)) {
		PHP_WINSYSTEM_RESTORE_ERRORS
		return;
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	mutex_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	mutex_attributes.lpSecurityDescriptor = NULL;
	mutex_attributes.bInheritHandle = inherit;

	if (use_unicode) {
		mutex_handle = CreateMutexW(&mutex_attributes, own, php_winsystem_unicode_get_wchar(&unicode TSRMLS_CC));
	} else {
		mutex_handle = CreateMutexA(&mutex_attributes, own, name);
	}
	error_num = GetLastError();

	if(own && error_num == ERROR_ALREADY_EXISTS) {
		zend_throw_exception(ce_winsystem_runtimeexception, "Mutex could not be created and marked as owned", 0 TSRMLS_CC);
		return;
	}

	if (mutex_handle == NULL) {
		if (error_num == ERROR_INVALID_HANDLE) {
			zend_throw_exception(ce_winsystem_runtimeexception, "Name is already in use for waitable object", error_num TSRMLS_CC);
		} else {
			winsystem_create_error(error_num, ce_winsystem_runtimeexception TSRMLS_CC);
		}
		return;
	}

	mutex->handle = mutex_handle;
	mutex->can_inherit = inherit;
	mutex->is_constructed = TRUE;
	if(name) {
		mutex->name.string = estrdup(name);
	} else if (use_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(unicode);
		mutex->name.unicode_object = unicode;
		mutex->is_unicode = TRUE;
	}
}
/* }}} */

/* {{{ proto object Win\System\Mutex::open(string|Unicode name[, inherit])
       attempts to open an existing mutex */
PHP_METHOD(WinSystemMutex, open)
{
	/* Used for regular string */
	char * name = NULL;
	int name_length;

	/* Used for wchar string */
	zval *unicode = NULL;
	int use_unicode = 0;

	/* Variables for a and w versions */
	zend_bool inherit = TRUE;
	HANDLE mutex_handle;
	winsystem_mutex_object *mutex;

	PHP_WINSYSTEM_EXCEPTIONS
	/* version one, use unicode */
	if (FAILURE != zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|b", &unicode, ce_winsystem_unicode, &inherit)) {
		use_unicode = 1;
	} else if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &name, &name_length, &inherit)) {
		PHP_WINSYSTEM_RESTORE_ERRORS
		return;
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	object_init_ex(return_value, ce_winsystem_mutex);
	mutex = (winsystem_mutex_object *)zend_object_store_get_object(return_value TSRMLS_CC);

	if (use_unicode) {
		mutex_handle = OpenMutexW(SYNCHRONIZE, inherit, php_winsystem_unicode_get_wchar(&unicode TSRMLS_CC));
	} else {
		mutex_handle = OpenMutexA(SYNCHRONIZE, inherit, name);
	}

	/* mutex is STILL null, we couldn't create the mutex, fail */
	if (mutex_handle == NULL) {
		DWORD error_num = GetLastError();

		if (error_num == ERROR_FILE_NOT_FOUND) {
			zend_throw_exception(ce_winsystem_runtimeexception, "Mutex was not found and could not be opened", error_num TSRMLS_CC);
		} else {
			winsystem_create_error(error_num, ce_winsystem_runtimeexception TSRMLS_CC);
		}
		return;
	}

	mutex->handle = mutex_handle;
	mutex->can_inherit = inherit;
	mutex->is_constructed = TRUE;
	if(name) {
		mutex->name.string = estrdup(name);
	} else if (use_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(unicode);
		mutex->name.unicode_object = unicode;
		mutex->is_unicode = TRUE;
	}
}
/* }}} */

/* {{{ proto mixed Win\System\Mutex->getName()
       returns the current name of the mutex (including a unicode
	   object if one was used) or null if it's unnamed */
PHP_METHOD(WinSystemMutex, getName)
{
	winsystem_mutex_object *mutex_object = (winsystem_mutex_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	PHP_WINSYSTEM_EXCEPTIONS
	if (FAILURE == zend_parse_parameters_none()) {
		PHP_WINSYSTEM_RESTORE_ERRORS
		return;
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	if (mutex_object->is_unicode) {
		RETURN_ZVAL(mutex_object->name.unicode_object, 1, 0);
	} else if (mutex_object->name.string) {
		RETURN_STRING(mutex_object->name.string, 1);
	}
	RETURN_NULL();
}
/* }}} */

/* {{{ proto bool Win\System\Mutex->release()
       releases a mutex - will fail if the mutex is not owned */
PHP_METHOD(WinSystemMutex, release)
{
	BOOL worked;
	winsystem_mutex_object *mutex = (winsystem_mutex_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	PHP_WINSYSTEM_EXCEPTIONS
	if (FAILURE == zend_parse_parameters_none()) {
		PHP_WINSYSTEM_RESTORE_ERRORS
		return;
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	worked = ReleaseMutex(mutex->handle);
	if (worked == 0) {
		RETURN_FALSE
	}

	RETURN_TRUE
}
/* }}} */

/* {{{ proto bool Win\System\Mutex->canInherit()
      if a mutex was created with a "can inherit" flag, meaning child processes can grab it */
PHP_METHOD(WinSystemMutex, canInherit)
{
	winsystem_mutex_object *mutex = (winsystem_mutex_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	PHP_WINSYSTEM_EXCEPTIONS
	if (FAILURE == zend_parse_parameters_none()) {
		PHP_WINSYSTEM_RESTORE_ERRORS
		return;
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	RETURN_BOOL(mutex->can_inherit)
}
/* }}} */

/* register mutex methods */
static zend_function_entry winsystem_mutex_functions[] = {
	PHP_ME(WinSystemMutex, __construct, WinSystemMutex___construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(WinSystemMutex, open, WinSystemMutex_open_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemMutex, release, WinSystemMutex_release_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemMutex, getName, WinSystemMutex_getName_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemMutex, canInherit, WinSystemMutex_canInherit_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, wait, WinSystemWaitable_wait_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, waitMsg, WinSystemWaitable_waitMsg_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, signalAndWait, WinSystemWaitable_signalAndWait_args, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Mutex Object Magic LifeCycle Functions
------------------------------------------------------------------*/

/* {{{ winsystem_mutex_construction_wrapper
       wraps around the constructor to make sure parent::__construct is always called  */
static void winsystem_mutex_construction_wrapper(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *this = getThis();
	winsystem_mutex_object *tobj;
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
		zend_throw_exception_ex(ce_winsystem_runtimeexception, 0 TSRMLS_CC,
			"parent::__construct() must be called in %s::__construct()", this_ce->name);
	efree(fci.params);
	zval_ptr_dtor(&retval_ptr);
}
/* }}} */

/* {{{ winsystem_mutex_get_constructor
       gets the constructor for the class  */
static zend_function *winsystem_mutex_get_constructor(zval *object TSRMLS_DC)
{
	/* Could always return constr_wrapper_fun, but it's uncessary to call the
	 * wrapper if instantiating the superclass */
	if (Z_OBJCE_P(object) == ce_winsystem_mutex)
		return zend_get_std_object_handlers()->
			get_constructor(object TSRMLS_CC);
	else
		return &winsystem_mutex_constructor_wrapper;
}
/* }}} */

/* {{{ winsystem_mutex_object_free
       frees up the mutex handle underneath as well as any stored
       unicode object or string name */
static void winsystem_mutex_object_free(void *object TSRMLS_DC)
{
	winsystem_mutex_object *mutex_object = (winsystem_mutex_object *)object;

	zend_object_std_dtor(&mutex_object->std TSRMLS_CC);

	if (mutex_object->is_unicode) {
		/* this will delref and clean up if refcount is 0 */
		zval_ptr_dtor(&mutex_object->name.unicode_object);
	} else if (mutex_object->name.string) {
		efree(mutex_object->name.string);
	}

	if(mutex_object->handle != NULL){
		CloseHandle(mutex_object->handle);
	}
	
	efree(mutex_object);
}
/* }}} */

/* {{{ winsystem_mutex_object_create
       object that has an internal HANDLE stored  */
static zend_object_value winsystem_mutex_object_create(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value          retval;
	winsystem_mutex_object     *mutex_object;
 
	mutex_object = ecalloc(1, sizeof(winsystem_mutex_object));
	zend_object_std_init(&mutex_object->std, ce TSRMLS_CC);
	mutex_object->handle = NULL;
	mutex_object->is_constructed = FALSE;
	mutex_object->can_inherit = FALSE;
	mutex_object->is_unicode = FALSE;
 
	object_properties_init(&mutex_object->std, ce);
 
	retval.handle = zend_objects_store_put(mutex_object,
		(zend_objects_store_dtor_t) zend_objects_destroy_object,
		(zend_objects_free_object_storage_t) winsystem_mutex_object_free,
		NULL TSRMLS_CC);
	retval.handlers = &winsystem_mutex_object_handlers;
	return retval;
}
/* }}} */

/* {{{ winsystem_mutex_object_clone
       duplicates the internal handle object, string dups a string
       or add refs an object */
static zend_object_value winsystem_mutex_object_clone(zval *this_ptr TSRMLS_DC)
{
	zend_object_value          retval;
	winsystem_mutex_object     *new_mutex_object;
	winsystem_mutex_object     *old_mutex_object = (winsystem_mutex_object *) zend_object_store_get_object(this_ptr TSRMLS_CC);
 
	new_mutex_object = ecalloc(1, sizeof(winsystem_mutex_object));
	zend_object_std_init(&new_mutex_object->std, old_mutex_object->std.ce TSRMLS_CC);
	DuplicateHandle(GetCurrentProcess(), 
					old_mutex_object->handle, 
					GetCurrentProcess(),
					&new_mutex_object->handle, 
					0, /* Ignored, we're using same_access */
					old_mutex_object->can_inherit,
					DUPLICATE_SAME_ACCESS);

	new_mutex_object->can_inherit = old_mutex_object->can_inherit;
	new_mutex_object->is_constructed = TRUE;
	if (old_mutex_object->is_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(old_mutex_object->name.unicode_object);
		new_mutex_object->name.unicode_object = old_mutex_object->name.unicode_object;
		new_mutex_object->is_unicode = TRUE;
	} else if (old_mutex_object->name.string) {
		new_mutex_object->name.string = estrdup(old_mutex_object->name.string);
	}
 
	object_properties_init(&new_mutex_object->std, old_mutex_object->std.ce);
 
	retval.handle = zend_objects_store_put(new_mutex_object,
		(zend_objects_store_dtor_t) zend_objects_destroy_object,
		(zend_objects_free_object_storage_t) winsystem_mutex_object_free,
		NULL TSRMLS_CC);
	retval.handlers = &winsystem_mutex_object_handlers;

	zend_objects_clone_members(&new_mutex_object->std, retval, zend_objects_get_address(this_ptr TSRMLS_CC), Z_OBJ_HANDLE_P(this_ptr) TSRMLS_CC);
	return retval; 
}
/* }}} */

/* {{{ winsystem_mutex_get_debug_info
       pretty var_dumps  */
static HashTable *winsystem_mutex_get_debug_info(zval *obj, int *is_temp TSRMLS_DC)
{
	winsystem_mutex_object *mutex = (winsystem_mutex_object *) zend_object_store_get_object(obj TSRMLS_CC);

	HashTable *retval, *std_props;
	zval *tmp;
	char *can_inherit, *name;
	int can_inherit_len, name_len;

	ALLOC_HASHTABLE(retval);
	zend_hash_init(retval, 2, NULL, ZVAL_PTR_DTOR, 0);

	std_props = zend_std_get_properties(obj TSRMLS_CC);
	zend_hash_copy(retval, std_props, (copy_ctor_func_t)zval_add_ref, NULL, sizeof(zval*));

	zend_mangle_property_name(&can_inherit, &can_inherit_len, 
	mutex->std.ce->name, mutex->std.ce->name_length, "canInherit", sizeof("canInherit") -1, 0);
	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, mutex->can_inherit);
	zend_hash_update(retval, can_inherit, can_inherit_len + 1, &tmp, sizeof(zval*), NULL);
	efree(can_inherit);

	zend_mangle_property_name(&name, &name_len, 
	mutex->std.ce->name, mutex->std.ce->name_length, "name", sizeof("name") -1, 0);
	if (mutex->is_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(mutex->name.unicode_object);
		zend_hash_update(retval, name, name_len + 1, &mutex->name.unicode_object, sizeof(zval*), NULL);
	} else {
		MAKE_STD_ZVAL(tmp);
		ZVAL_STRING(tmp, mutex->name.string, 1);
		zend_hash_update(retval, name, name_len + 1, &tmp, sizeof(zval*), NULL);
	}
	efree(name);

	*is_temp = 1;
	return retval; 
}
/* }}} */


/* ----------------------------------------------------------------
  Win\System\Mutex LifeCycle Functions
------------------------------------------------------------------*/
PHP_MINIT_FUNCTION(winsystem_mutex)
{
	zend_class_entry ce;

	memcpy(&winsystem_mutex_object_handlers, zend_get_std_object_handlers(),
		sizeof winsystem_mutex_object_handlers);
	winsystem_mutex_object_handlers.get_constructor = winsystem_mutex_get_constructor;
	winsystem_mutex_object_handlers.clone_obj = winsystem_mutex_object_clone;
	winsystem_mutex_object_handlers.get_debug_info = winsystem_mutex_get_debug_info;

	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Mutex", winsystem_mutex_functions);
	ce_winsystem_mutex = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(ce_winsystem_mutex TSRMLS_CC, 1, ce_winsystem_waitable);
	ce_winsystem_mutex->create_object = winsystem_mutex_object_create;

	winsystem_mutex_constructor_wrapper.type = ZEND_INTERNAL_FUNCTION;
	winsystem_mutex_constructor_wrapper.common.function_name = "internal_construction_wrapper";
	winsystem_mutex_constructor_wrapper.common.scope = ce_winsystem_mutex;
	winsystem_mutex_constructor_wrapper.common.fn_flags = ZEND_ACC_PROTECTED;
	winsystem_mutex_constructor_wrapper.common.prototype = NULL;
	winsystem_mutex_constructor_wrapper.common.required_num_args = 0;
	winsystem_mutex_constructor_wrapper.common.arg_info = NULL;
	winsystem_mutex_constructor_wrapper.internal_function.handler = winsystem_mutex_construction_wrapper;
	winsystem_mutex_constructor_wrapper.internal_function.module = EG(current_module);

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
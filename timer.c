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
#include "waitable.h"

zend_class_entry *ce_winsystem_timer;
static zend_object_handlers winsystem_timer_object_handlers;
static zend_function winsystem_timer_constructor_wrapper;

VOID CALLBACK php_winsystem_timer_callback(LPVOID param, DWORD timerlow, DWORD timerhigh);

struct _winsystem_timer_callback {
	zend_fcall_info callback_info;
	zend_fcall_info_cache callback_cache;
	char *src_filename;
	uint src_lineno;
	int refcount;
#ifdef ZTS
	TSRMLS_D;
#endif
};

struct _winsystem_timer_object {
	zend_object    std;
	zend_bool      is_constructed;
	HANDLE         handle;
	BOOL           can_inherit;
	zend_bool      is_unicode;
	winsystem_name name;
	winsystem_timer_callback *store;
};

/* ----------------------------------------------------------------
  Win\System\Timer Userland API
------------------------------------------------------------------*/
ZEND_BEGIN_ARG_INFO_EX(WinSystemTimer___construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, auto_reset)
	ZEND_ARG_INFO(0, process_inherit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(WinSystemTimer_open_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, process_inherit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemTimer_cancel_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemTimer_set_args, ZEND_SEND_BY_VAL)
	ZEND_ARG_INFO(0, interval)
	ZEND_ARG_INFO(0, period)
	ZEND_ARG_INFO(0, resume)
	ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemTimer_getName_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemTimer_canInherit_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

/* {{{ proto bool Win\System\Timer->__construct()
       creates a new timer */
PHP_METHOD(WinSystemTimer, __construct)
{
	/* Used for regular string */
	char * name = NULL;
	int name_length;

	/* Used for wchar string */
	zval *unicode = NULL;
	int use_unicode = 0;

	SECURITY_ATTRIBUTES timer_attributes;
	zend_bool inherit = TRUE;
	zend_bool autoreset = TRUE;
	HANDLE timer_handle;
	winsystem_timer_object *timer = (winsystem_timer_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	PHP_WINSYSTEM_EXCEPTIONS
	/* version one, use unicode */
	if (FAILURE != zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|bb", &unicode, ce_winsystem_unicode, &autoreset, &inherit)) {
		use_unicode = 1;
	} else if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!bb", &name, &name_length, &autoreset, &inherit)) {
		PHP_WINSYSTEM_RESTORE_ERRORS
		return;
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	timer_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	timer_attributes.lpSecurityDescriptor = NULL;
	timer_attributes.bInheritHandle = inherit;

	if (use_unicode) {
		timer_handle = CreateWaitableTimerW(&timer_attributes, autoreset, php_winsystem_unicode_get_wchar(&unicode TSRMLS_CC));
	} else {
		timer_handle = CreateWaitableTimerA(&timer_attributes, autoreset, name);
	}

	/* timer is STILL null, we couldn't create the timer, fail */
	if (timer_handle == NULL) {
		zend_throw_exception(ce_winsystem_runtimeexception, "Could not create or open the requested timer", 0 TSRMLS_CC);
		return;
	}

	timer->handle = timer_handle;
	timer->can_inherit = inherit;
	timer->is_constructed = TRUE;
	if(name) {
		timer->name.string = estrdup(name);
	} else if (use_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(unicode);
		timer->name.unicode_object = unicode;
		timer->is_unicode = TRUE;
	}
}
/* }}} */

/* {{{ proto object Win\System\Timer::open(string|Unicode name[, inherit])
       attempts to open an existing timer */
PHP_METHOD(WinSystemTimer, open)
{
	/* Used for regular string */
	char * name = NULL;
	int name_length;

	/* Used for wchar string */
	zval *unicode = NULL;
	int use_unicode = 0;

	/* Variables for a and w versions */
	zend_bool inherit = TRUE;
	HANDLE timer_handle;
	winsystem_timer_object *timer;

	PHP_WINSYSTEM_EXCEPTIONS
	/* version one, use unicode */
	if (FAILURE != zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|b", &unicode, ce_winsystem_unicode, &inherit)) {
		use_unicode = 1;
	} else if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &name, &name_length, &inherit)) {
		PHP_WINSYSTEM_RESTORE_ERRORS
		return;
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	object_init_ex(return_value, ce_winsystem_event);
	timer = (winsystem_timer_object *)zend_object_store_get_object(return_value TSRMLS_CC);

	if (use_unicode) {
		timer_handle = OpenWaitableTimerW(SYNCHRONIZE, inherit, php_winsystem_unicode_get_wchar(&unicode TSRMLS_CC));
	} else {
		timer_handle = OpenWaitableTimerA(SYNCHRONIZE, inherit, name);
	}

	if (timer_handle == NULL) {
		DWORD error_num = GetLastError();

		if (error_num == ERROR_FILE_NOT_FOUND) {
			zend_throw_exception(ce_winsystem_runtimeexception, "Timer was not found and could not be opened", error_num TSRMLS_CC);
		} else {
			winsystem_create_error(error_num, ce_winsystem_runtimeexception TSRMLS_CC);
		}
		return;
	}

	timer->handle = timer_handle;
	timer->can_inherit = inherit;
	timer->is_constructed = TRUE;
	if(name) {
		timer->name.string = estrdup(name);
	} else if (use_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(unicode);
		timer->name.unicode_object = unicode;
		timer->is_unicode = TRUE;
	}
}
/* }}} */

/* {{{ proto bool Win\System\Timer->getName()
       returns the current name of the timer or null if it's unnamed */
PHP_METHOD(WinSystemTimer, getName)
{
	winsystem_timer_object *timer = (winsystem_timer_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	PHP_WINSYSTEM_EXCEPTIONS
	if (FAILURE == zend_parse_parameters_none()) {
		PHP_WINSYSTEM_RESTORE_ERRORS
		return;
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	if (timer->is_unicode) {
		RETURN_ZVAL(timer->name.unicode_object, 1, 0);
	} else if (timer->name.string) {
		RETURN_STRING(timer->name.string, 1)
	}
	RETURN_NULL()
}
/* }}} */

/* {{{ proto bool Win\System\Timer->cancel()
       Sets the specified waitable timer to the inactive state */
PHP_METHOD(WinSystemTimer, cancel)
{
	winsystem_timer_object *timer_object = (winsystem_timer_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	PHP_WINSYSTEM_EXCEPTIONS
	if (FAILURE == zend_parse_parameters_none()) {
		PHP_WINSYSTEM_RESTORE_ERRORS
		return;
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	if (timer_object->store != NULL) {
		if (timer_object->store->callback_info.params) {
			zval ***params  = timer_object->store->callback_info.params;
			int param_count = timer_object->store->callback_info.param_count;

				while (--param_count) {
					zval_ptr_dtor(params[param_count]);
				}
		}

		zval_ptr_dtor(&(timer_object->store->callback_info.function_name));
		if (timer_object->store->callback_info.object_ptr) {
			zval_ptr_dtor(&(timer_object->store->callback_info.object_ptr));
		}
	}
	timer_object->store = NULL;

	RETURN_BOOL(CancelWaitableTimer(timer_object->handle));
}
/* }}} */

/* {{{ proto bool Win\System\Timer->set(int interval[, int period, bool resume, callback timerfunction, arg1...])
       Adds a callback to the waitable timer */
PHP_METHOD(WinSystemTimer, set)
{
	long period = 0, interval = 0;
	LARGE_INTEGER duetime;
	zend_bool resume = FALSE;
	zend_fcall_info finfo;
	zend_fcall_info_cache fcache;
	zval ***args = NULL;
	int argc = 0, i = 0;
	PTIMERAPCROUTINE entry = NULL;
	winsystem_timer_callback *store = NULL;
	winsystem_timer_object *timer = (winsystem_timer_object*)zend_object_store_get_object(getThis() TSRMLS_CC);

	PHP_WINSYSTEM_EXCEPTIONS
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|lbf*", &interval, &period, &resume, &finfo, &fcache, &args, &argc)) {
		PHP_WINSYSTEM_RESTORE_ERRORS
		return;
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	/* This is dirty as sin, but I need to be able to tell if we hit a callback */
	finfo.size = 0;

	/* the earlier hack makes this not blow up, even if no callback is hit */
	if (finfo.size > 0) {
		if (argc) {
			for(i = 0; i < argc; i++) {
					zval **newppzval = emalloc(sizeof(zval*));
					MAKE_STD_ZVAL(*newppzval);
					ZVAL_ZVAL(*newppzval, *args[i], 1, 0);
					Z_SET_REFCOUNT_PP(newppzval, 2);
					args[i] = newppzval;
			}
		}

		store = emalloc(sizeof(winsystem_timer_callback));

		store->callback_info = finfo;
		if (store->callback_info.object_ptr) {
			zval_add_ref(&(store->callback_info.object_ptr));
		}
		zval_add_ref(&(store->callback_info.function_name));
		store->callback_cache = fcache;

		store->src_filename = estrdup(zend_get_executed_filename(TSRMLS_C));
		store->src_lineno = zend_get_executed_lineno(TSRMLS_C);
		store->callback_info.params      = args;
		store->callback_info.param_count = argc;
		store->refcount = 0;
#ifdef ZTS
		store->TSRMLS_C = TSRMLS_C;
#endif
		timer->store = store;
		entry = php_winsystem_timer_callback;
	}

	duetime.QuadPart = (-((__int64)interval)) * 10LL;
	RETURN_BOOL(SetWaitableTimer(timer->handle, &duetime, period, entry, store, resume));
}
/* }}} */

/* {{{ proto bool Win\System\Timer->canInherit()
      if a timer was created with a "can inherit" flag, meaning child processes can grab it */
PHP_METHOD(WinSystemTimer, canInherit)
{
	winsystem_timer_object *timer = (winsystem_timer_object *)zend_object_store_get_object(getThis() TSRMLS_CC);

	PHP_WINSYSTEM_EXCEPTIONS
	if (FAILURE == zend_parse_parameters_none()) {
		PHP_WINSYSTEM_RESTORE_ERRORS
		return;
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	RETURN_BOOL(timer->can_inherit)
}
/* }}} */

/* register timer methods */
static zend_function_entry winsystem_timer_functions[] = {
	PHP_ME(WinSystemTimer, __construct, WinSystemTimer___construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(WinSystemTimer, open, WinSystemTimer_open_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemTimer, cancel, WinSystemTimer_cancel_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemTimer, set, WinSystemTimer_set_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemTimer, getName, WinSystemTimer_getName_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemTimer, canInherit, WinSystemTimer_canInherit_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, wait, WinSystemWaitable_wait_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, waitMsg, WinSystemWaitable_waitMsg_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, signalAndWait, WinSystemWaitable_signalAndWait_args, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Timer C API
------------------------------------------------------------------*/

VOID CALLBACK php_winsystem_timer_callback(LPVOID param, DWORD timerlow, DWORD timerhigh)
{
	if (param) {
		zval *retval_ptr = NULL;
		winsystem_timer_callback * store = (winsystem_timer_callback *) param;
#ifdef ZTS
		TSRMLS_D = store->TSRMLS_C;
#endif
		store->callback_info.retval_ptr_ptr = &retval_ptr;
		if (FAILURE == zend_call_function(&(store->callback_info), &(store->callback_cache) TSRMLS_CC)) {
			php_error(E_RECOVERABLE_ERROR, "Error calling %s", store->callback_info.function_name);
		}
	}
}

/* ----------------------------------------------------------------
  Win\System\Timer Object Magic LifeCycle Functions
------------------------------------------------------------------*/

/* {{{ winsystem_timer_construction_wrapper
       wraps around the constructor to make sure parent::__construct is always called  */
static void winsystem_timer_construction_wrapper(INTERNAL_FUNCTION_PARAMETERS)
{
	zval *this = getThis();
	winsystem_timer_object *tobj;
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

/* {{{ winsystem_timer_get_constructor
       gets the constructor for the class  */
static zend_function *winsystem_timer_get_constructor(zval *object TSRMLS_DC)
{
	/* Could always return constr_wrapper_fun, but it's uncessary to call the
	 * wrapper if instantiating the superclass */
	if (Z_OBJCE_P(object) == ce_winsystem_timer)
		return zend_get_std_object_handlers()->
			get_constructor(object TSRMLS_CC);
	else
		return &winsystem_timer_constructor_wrapper;
}
/* }}} */

/* {{{ winsystem_timer_object_free
       frees up the event handle underneath as well as any stored
       unicode object or string name */
static void winsystem_timer_object_free(void *object TSRMLS_DC)
{
	winsystem_timer_object *timer_object = (winsystem_timer_object *)object;

	zend_object_std_dtor(&timer_object->std TSRMLS_CC);

	if (timer_object->is_unicode) {
		Z_DELREF_P(timer_object->name.unicode_object);
	} else if (timer_object->name.string) {
		efree(timer_object->name.string);
	}

	if (timer_object->store != NULL) {
		if (timer_object->store->refcount > 0) {
			timer_object->store->refcount--;
			timer_object->store = NULL;
		} else {
			if (timer_object->store->callback_info.params) {
				zval ***params  = timer_object->store->callback_info.params;
				int param_count = timer_object->store->callback_info.param_count;

					while (--param_count) {
						zval_ptr_dtor(params[param_count]);
					}
			}

			zval_ptr_dtor(&(timer_object->store->callback_info.function_name));
			if (timer_object->store->callback_info.object_ptr) {
				zval_ptr_dtor(&(timer_object->store->callback_info.object_ptr));
			}
			if (timer_object->handle != NULL) {
				/* This might fail if the timer is not set, but we don't care */
				CancelWaitableTimer(timer_object->handle);
			}
		}
	}

	if(timer_object->handle != NULL){
		CloseHandle(timer_object->handle);
	}
	
	efree(timer_object);
}
/* }}} */

/* {{{ winsystem_timer_object_create
       object that has an internal HANDLE stored  */
static zend_object_value winsystem_timer_object_create(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value          retval;
	winsystem_timer_object     *timer_object;
 
	timer_object = ecalloc(1, sizeof(winsystem_timer_object));
	zend_object_std_init(&timer_object->std, ce TSRMLS_CC);
	timer_object->handle = NULL;
	timer_object->is_constructed = FALSE;
	timer_object->can_inherit = FALSE;
	timer_object->is_unicode = FALSE;
	timer_object->store = NULL;

	object_properties_init(&timer_object->std, ce);

	retval.handle = zend_objects_store_put(timer_object,
		(zend_objects_store_dtor_t) zend_objects_destroy_object,
		(zend_objects_free_object_storage_t) winsystem_timer_object_free,
		NULL TSRMLS_CC);
	retval.handlers = &winsystem_timer_object_handlers;
	return retval;
}
/* }}} */

/* {{{ winsystem_timer_object_clone
       duplicates the internal handle object, string dups a string
	   or add refs an object */
static zend_object_value winsystem_timer_object_clone(zval *this_ptr TSRMLS_DC)
{
	zend_object_value          retval;
	winsystem_timer_object     *new_timer_object;
	winsystem_timer_object     *old_timer_object = (winsystem_timer_object *) zend_object_store_get_object(this_ptr TSRMLS_CC);
 
	new_timer_object = ecalloc(1, sizeof(winsystem_timer_object));
	zend_object_std_init(&new_timer_object->std, old_timer_object->std.ce TSRMLS_CC);
	DuplicateHandle(GetCurrentProcess(), 
					old_timer_object->handle, 
					GetCurrentProcess(),
					&new_timer_object->handle, 
					0, /* Ignored, we're using same_access */
					old_timer_object->can_inherit,
					DUPLICATE_SAME_ACCESS);

	new_timer_object->can_inherit = old_timer_object->can_inherit;
	new_timer_object->is_constructed = TRUE;
	if (old_timer_object->store) {
		new_timer_object->store = old_timer_object->store;
		new_timer_object->store->refcount++;
	} else {
		new_timer_object->store = NULL;
	}

	if (old_timer_object->is_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(old_timer_object->name.unicode_object);
		new_timer_object->name.unicode_object = old_timer_object->name.unicode_object;
		new_timer_object->is_unicode = TRUE;
	} else if (old_timer_object->name.string) {
		new_timer_object->name.string = estrdup(old_timer_object->name.string);
	}
 
	object_properties_init(&new_timer_object->std, old_timer_object->std.ce);
 
	retval.handle = zend_objects_store_put(new_timer_object,
		(zend_objects_store_dtor_t) zend_objects_destroy_object,
		(zend_objects_free_object_storage_t) winsystem_timer_object_free,
		NULL TSRMLS_CC);
	retval.handlers = &winsystem_timer_object_handlers;

	zend_objects_clone_members(&new_timer_object->std, retval, zend_objects_get_address(this_ptr TSRMLS_CC), Z_OBJ_HANDLE_P(this_ptr) TSRMLS_CC);
	return retval;
}
/* }}} */

/* {{{ winsystem_timer_get_debug_info
       pretty var_dumps  */
static HashTable *winsystem_timer_get_debug_info(zval *obj, int *is_temp TSRMLS_DC)
{
	winsystem_timer_object *timer = (winsystem_timer_object *) zend_object_store_get_object(obj TSRMLS_CC);

	HashTable *retval, *std_props;
	zval *tmp;
	char *can_inherit, *name;
	int can_inherit_len, name_len;
	ALLOC_HASHTABLE(retval);
	zend_hash_init(retval, 2, NULL, ZVAL_PTR_DTOR, 0);

	std_props = zend_std_get_properties(obj TSRMLS_CC);
	zend_hash_copy(retval, std_props, (copy_ctor_func_t)zval_add_ref, NULL, sizeof(zval*));

	zend_mangle_property_name(&can_inherit, &can_inherit_len, 
	timer->std.ce->name, timer->std.ce->name_length, "canInherit", sizeof("canInherit") -1, 0);
	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, timer->can_inherit);
	zend_hash_update(retval, can_inherit, can_inherit_len + 1, &tmp, sizeof(zval*), NULL);
	efree(can_inherit);

	zend_mangle_property_name(&name, &name_len, 
	timer->std.ce->name, timer->std.ce->name_length, "name", sizeof("name") -1, 0);
	if (timer->is_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(timer->name.unicode_object);
		zend_hash_update(retval, name, name_len + 1, &timer->name.unicode_object, sizeof(zval*), NULL);
	} else {
		MAKE_STD_ZVAL(tmp);
		ZVAL_STRING(tmp, timer->name.string, 1);
		zend_hash_update(retval, name, name_len + 1, &tmp, sizeof(zval*), NULL);
	}
	efree(name);

	*is_temp = 1;
	return retval; 
}
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Timer LifeCycle Functions
------------------------------------------------------------------*/
PHP_MINIT_FUNCTION(winsystem_timer)
{
	zend_class_entry ce;

	memcpy(&winsystem_timer_object_handlers, zend_get_std_object_handlers(),
		sizeof winsystem_timer_object_handlers);
	winsystem_timer_object_handlers.get_constructor = winsystem_timer_get_constructor;
	winsystem_timer_object_handlers.clone_obj = winsystem_timer_object_clone;
	winsystem_timer_object_handlers.get_debug_info = winsystem_timer_get_debug_info;
	
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Timer", winsystem_timer_functions);
	ce_winsystem_timer = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(ce_winsystem_timer TSRMLS_CC, 1, ce_winsystem_waitable);
	ce_winsystem_timer->create_object = winsystem_timer_object_create;
 
	winsystem_timer_constructor_wrapper.type = ZEND_INTERNAL_FUNCTION;
	winsystem_timer_constructor_wrapper.common.function_name = "internal_construction_wrapper";
	winsystem_timer_constructor_wrapper.common.scope = ce_winsystem_timer;
	winsystem_timer_constructor_wrapper.common.fn_flags = ZEND_ACC_PROTECTED;
	winsystem_timer_constructor_wrapper.common.prototype = NULL;
	winsystem_timer_constructor_wrapper.common.required_num_args = 0;
	winsystem_timer_constructor_wrapper.common.arg_info = NULL;
	winsystem_timer_constructor_wrapper.internal_function.handler = winsystem_timer_construction_wrapper;
	winsystem_timer_constructor_wrapper.internal_function.module = EG(current_module);

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
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
#include "zend_exceptions.h"
#include "implement_waitable.h"

zend_class_entry *ce_winsystem_event;
static zend_object_handlers winsystem_event_object_handlers;
static zend_function winsystem_event_constructor_wrapper;

/* ----------------------------------------------------------------
  Win\System\Event Userland API                                                    
------------------------------------------------------------------*/

ZEND_BEGIN_ARG_INFO_EX(WinSystemEvent___construct_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, initial_state)
	ZEND_ARG_INFO(0, auto_reset)
    ZEND_ARG_INFO(0, process_inherit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(WinSystemEvent_open_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, name)
    ZEND_ARG_INFO(0, process_inherit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemEvent_reset_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemEvent_set_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemEvent_pulse_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemEvent_getName_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemEvent_canInherit_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

/* {{{ proto Win\System\Event Win\System\Event->__construct()
       creates a new event */
PHP_METHOD(WinSystemEvent, __construct)
{
	/* Used for regular string */
	char * name = NULL;
	int name_length;

	/* Used for wchar string */
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

	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	/* version one, use unicode */
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "O|bbb", &unicode, ce_winsystem_unicode, &state, &autoreset, &inherit) != FAILURE) {
		use_unicode = 1;
		unicode_object = (winsystem_unicode_object *)zend_object_store_get_object(unicode TSRMLS_CC);
	} else if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!bbb", &name, &name_length, &state, &autoreset, &inherit) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	event_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	event_attributes.lpSecurityDescriptor = NULL;
	event_attributes.bInheritHandle = inherit;

	if (use_unicode) {
		event_handle = CreateEventW(&event_attributes, state, autoreset, unicode_object->unicode_string);
	} else {
		event_handle = CreateEventA(&event_attributes, state, autoreset, name);
	}

	/* event is STILL null, we couldn't create the event, fail */
	if (event_handle == NULL) {
		DWORD error_num = GetLastError();

		if (error_num == ERROR_INVALID_HANDLE) {
			zend_throw_exception(ce_winsystem_exception, "Name is already in use for waitable object", error_num TSRMLS_CC);
		} else {
			winsystem_create_error(error_num, ce_winsystem_exception TSRMLS_CC);
		}
		return;
	}

	event->handle = event_handle;
	event->can_inherit = inherit;
	event->is_constructed = TRUE;
	if(name) {
		event->name.string = estrdup(name);
	} else if (use_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(unicode);
		event->name.unicode_object = unicode;
		event->is_unicode = TRUE;
	}
}
/* }}} */

/* {{{ proto object Win\System\Event::open(string|Unicode name[, inherit])
       attempts to open an existing event */
PHP_METHOD(WinSystemEvent, open)
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
	HANDLE event_handle;
	winsystem_event_object *event;
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

	object_init_ex(return_value, ce_winsystem_event);
	event = (winsystem_event_object *)zend_object_store_get_object(return_value TSRMLS_CC);

	if (use_unicode) {
		event_handle = OpenEventW(SYNCHRONIZE, inherit, unicode_object->unicode_string);
	} else {
		event_handle = OpenEventA(SYNCHRONIZE, inherit, name);
	}

	if (event_handle == NULL) {
		DWORD error_num = GetLastError();

		if (error_num == ERROR_FILE_NOT_FOUND) {
			zend_throw_exception(ce_winsystem_exception, "Event was not found and could not be opened", error_num TSRMLS_CC);
		} else {
			winsystem_create_error(error_num, ce_winsystem_exception TSRMLS_CC);
		}
		return;
	}

	event->handle = event_handle;
	event->can_inherit = inherit;
	event->is_constructed = TRUE;
	if(name) {
		event->name.string = estrdup(name);
	} else if (use_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(unicode);
		event->name.unicode_object = unicode;
		event->is_unicode = TRUE;
	}
}
/* }}} */

/* {{{ proto bool Win\System\Event->getName()
       returns the current name of the event or null if it's unnamed */
PHP_METHOD(WinSystemEvent, getName)
{
	zend_error_handling error_handling;
	winsystem_event_object *event = (winsystem_event_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (event->is_unicode) {
		RETURN_ZVAL(event->name.unicode_object, 1, 0);
	} else if (event->name.string) {
		RETURN_STRING(event->name.string, 1)
	}
	RETURN_NULL()
}
/* }}} */

/* {{{ proto bool Win\System\Event->reset()
       resets an event - not needed for autoreset signals */
PHP_METHOD(WinSystemEvent, reset)
{
	zend_error_handling error_handling;
	winsystem_event_object *event = (winsystem_event_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(ResetEvent(event->handle))
}
/* }}} */

/* {{{ proto bool Win\System\Event->set()
       sets event to "signaled" state */
PHP_METHOD(WinSystemEvent, set)
{
	zend_error_handling error_handling;
	winsystem_event_object *event = (winsystem_event_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(SetEvent(event->handle))
}
/* }}} */

/* {{{ proto bool Win\System\Event->pulse()
       pulses event - sets it to the on and then off state */
PHP_METHOD(WinSystemEvent, pulse)
{
	zend_error_handling error_handling;
	winsystem_event_object *event = (winsystem_event_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(PulseEvent(event->handle))
}
/* }}} */

/* {{{ proto bool Win\System\Event->canInherit()
      if a event was created with a "can inherit" flag, meaning child processes can grab it */
PHP_METHOD(WinSystemEvent, canInherit)
{
	zend_error_handling error_handling;
	winsystem_event_object *event = (winsystem_event_object *)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	RETURN_BOOL(event->can_inherit)
}
/* }}} */

/* register event methods */
static zend_function_entry winsystem_event_functions[] = {
	PHP_ME(WinSystemEvent, __construct, WinSystemEvent___construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(WinSystemEvent, open, WinSystemEvent_open_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemEvent, set, WinSystemEvent_set_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemEvent, reset, WinSystemEvent_reset_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemEvent, pulse, WinSystemEvent_pulse_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemEvent, getName, WinSystemEvent_getName_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemEvent, canInherit, WinSystemEvent_canInherit_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, wait, WinSystemWaitable_wait_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, waitMsg, WinSystemWaitable_waitMsg_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, signalAndWait, WinSystemWaitable_signalAndWait_args, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Event Object Magic LifeCycle Functions                                                    
------------------------------------------------------------------*/
/* {{{ winsystem_event_construction_wrapper
       wraps around the constructor to make sure parent::__construct is always called  */
static void winsystem_event_construction_wrapper(INTERNAL_FUNCTION_PARAMETERS) {
    zval *this = getThis();
    winsystem_event_object *tobj;
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

/* {{{ winsystem_event_get_constructor
       gets the constructor for the class  */
static zend_function *winsystem_event_get_constructor(zval *object TSRMLS_DC)
{
    /* Could always return constr_wrapper_fun, but it's uncessary to call the
     * wrapper if instantiating the superclass */
    if (Z_OBJCE_P(object) == ce_winsystem_event)
        return zend_get_std_object_handlers()->
            get_constructor(object TSRMLS_CC);
    else
        return &winsystem_event_constructor_wrapper;
}
/* }}} */

/* {{{ winsystem_event_object_free
       frees up the event handle underneath as well as any stored
	   unicode object or string name */
static void winsystem_event_object_free(void *object TSRMLS_DC)
{
	winsystem_event_object *event_object = (winsystem_event_object *)object;

	zend_object_std_dtor(&event_object->std TSRMLS_CC);

	if (event_object->is_unicode) {
		Z_DELREF_P(event_object->name.unicode_object);
	} else if (event_object->name.string) {
		efree(event_object->name.string);
	}

	if(event_object->handle != NULL){
		CloseHandle(event_object->handle);
	}
	
	efree(event_object);
}
/* }}} */

/* {{{ winsystem_event_object_create
       object that has an internal HANDLE stored  */
static zend_object_value winsystem_event_object_create(zend_class_entry *ce TSRMLS_DC)
{
    zend_object_value          retval;
    winsystem_event_object     *event_object;
 
	event_object = ecalloc(1, sizeof(winsystem_event_object));
    zend_object_std_init((zend_object *) event_object, ce TSRMLS_CC);
	event_object->handle = NULL;
	event_object->is_constructed = FALSE;
	event_object->can_inherit = FALSE;
	event_object->is_unicode = FALSE;
 
    zend_hash_copy(event_object->std.properties, &(ce->default_properties),
        (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval*));
 
    retval.handle = zend_objects_store_put(event_object,
        (zend_objects_store_dtor_t) zend_objects_destroy_object,
        (zend_objects_free_object_storage_t) winsystem_event_object_free,
        NULL TSRMLS_CC);
    retval.handlers = &winsystem_event_object_handlers;
    return retval;
}
/* }}} */

/* {{{ winsystem_event_object_clone
       duplicates the internal handle object, string dups a string
	   or add refs an object */
static zend_object_value winsystem_event_object_clone(zval *this_ptr TSRMLS_DC)
{
	zend_object_value          retval;
    winsystem_event_object     *new_event_object;
	winsystem_event_object     *old_event_object = (winsystem_event_object *) zend_object_store_get_object(this_ptr TSRMLS_CC);
 
	new_event_object = ecalloc(1, sizeof(winsystem_event_object));
    zend_object_std_init((zend_object *) new_event_object, old_event_object->std.ce TSRMLS_CC);
	DuplicateHandle(GetCurrentProcess(), 
                    old_event_object->handle, 
                    GetCurrentProcess(),
                    &new_event_object->handle, 
                    0, /* Ignored, we're using same_access */
                    old_event_object->can_inherit,
                    DUPLICATE_SAME_ACCESS);

	new_event_object->can_inherit = old_event_object->can_inherit;
	new_event_object->is_constructed = TRUE;

	if (old_event_object->is_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(old_event_object->name.unicode_object);
		new_event_object->name.unicode_object = old_event_object->name.unicode_object;
		new_event_object->is_unicode = TRUE;
	} else if (old_event_object->name.string) {
		new_event_object->name.string = estrdup(old_event_object->name.string);
	}
 
    zend_hash_copy(new_event_object->std.properties, &(old_event_object->std.ce->default_properties),
        (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval*));
 
    retval.handle = zend_objects_store_put(new_event_object,
        (zend_objects_store_dtor_t) zend_objects_destroy_object,
        (zend_objects_free_object_storage_t) winsystem_event_object_free,
        NULL TSRMLS_CC);
    retval.handlers = &winsystem_event_object_handlers;

	zend_objects_clone_members(&new_event_object->std, retval, zend_objects_get_address(this_ptr TSRMLS_CC), Z_OBJ_HANDLE_P(this_ptr) TSRMLS_CC);
	return retval; 
}
/* }}} */

/* {{{ winsystem_event_get_debug_info
       pretty var_dumps  */
static HashTable *winsystem_event_get_debug_info(zval *obj, int *is_temp TSRMLS_DC)
{
	winsystem_event_object *event = (winsystem_event_object *) zend_object_store_get_object(obj TSRMLS_CC);

	HashTable *retval;
	zval *tmp;
	char *can_inherit, *name;
	int can_inherit_len, name_len;

	ALLOC_HASHTABLE(retval);
    zend_hash_init(retval, 1, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(retval, event->std.properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));

	zend_mangle_property_name(&can_inherit, &can_inherit_len, 
	event->std.ce->name, event->std.ce->name_length, "canInherit", sizeof("canInherit") -1, 0);
	MAKE_STD_ZVAL(tmp);
	ZVAL_BOOL(tmp, event->can_inherit);
	zend_hash_update(retval, can_inherit, can_inherit_len + 1, &tmp, sizeof(zval*), NULL);
    efree(can_inherit);

	zend_mangle_property_name(&name, &name_len, 
	event->std.ce->name, event->std.ce->name_length, "name", sizeof("name") -1, 0);
	if (event->is_unicode) {
		/* ref our object and store it */
		Z_ADDREF_P(event->name.unicode_object);
		zend_hash_update(retval, name, name_len + 1, &event->name.unicode_object, sizeof(zval*), NULL);
	} else {
		MAKE_STD_ZVAL(tmp);
		ZVAL_STRING(tmp, event->name.string, 1);
		zend_hash_update(retval, name, name_len + 1, &tmp, sizeof(zval*), NULL);
	}
    efree(name);

	*is_temp = 1;
    return retval; 
}
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Event LifeCycle Functions                                                    
------------------------------------------------------------------*/
PHP_MINIT_FUNCTION(winsystem_event)
{
	zend_class_entry ce;

	memcpy(&winsystem_event_object_handlers, zend_get_std_object_handlers(),
        sizeof winsystem_event_object_handlers);
    winsystem_event_object_handlers.get_constructor = winsystem_event_get_constructor;
	winsystem_event_object_handlers.clone_obj = winsystem_event_object_clone;
	winsystem_event_object_handlers.get_debug_info = winsystem_event_get_debug_info;
	
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Event", winsystem_event_functions);
	ce_winsystem_event = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(ce_winsystem_event TSRMLS_CC, 1, ce_winsystem_waitable);
	ce_winsystem_event->create_object = winsystem_event_object_create;
 
    winsystem_event_constructor_wrapper.type = ZEND_INTERNAL_FUNCTION;
    winsystem_event_constructor_wrapper.common.function_name = "internal_construction_wrapper";
    winsystem_event_constructor_wrapper.common.scope = ce_winsystem_event;
    winsystem_event_constructor_wrapper.common.fn_flags = ZEND_ACC_PROTECTED;
    winsystem_event_constructor_wrapper.common.prototype = NULL;
    winsystem_event_constructor_wrapper.common.required_num_args = 0;
    winsystem_event_constructor_wrapper.common.arg_info = NULL;
    winsystem_event_constructor_wrapper.common.pass_rest_by_reference = 0;
    winsystem_event_constructor_wrapper.common.return_reference = 0;
    winsystem_event_constructor_wrapper.internal_function.handler = winsystem_event_construction_wrapper;
    winsystem_event_constructor_wrapper.internal_function.module = EG(current_module);

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
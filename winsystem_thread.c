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

#ifdef ZTS

#include "php_winsystem.h"

ZEND_DECLARE_MODULE_GLOBALS(winsystem);

#include "php_main.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/php_var.h" 
#include "implement_waitable.h"

/* All the classes in this file */
zend_class_entry *ce_winsystem_thread;

/* Truly global storage - but also thread safe */
TsHashTable winsystem_threads_globals;

DWORD WINAPI php_winsystem_thread_callback(LPVOID lpParam);
int php_winsystem_thread_copy_zval(zval **retval, zval *src, void ***prev_tsrm_ls TSRMLS_DC);
static void winsystem_finish_thread(void *data);

/* ----------------------------------------------------------------
  Win\System\Thread Userland API                                                    
------------------------------------------------------------------*/

ZEND_BEGIN_ARG_INFO(WinSystemThread_start_args, ZEND_SEND_BY_VAL)
    ZEND_ARG_INFO(0, callback)
	ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemThread_set_args, ZEND_SEND_BY_VAL)
	ZEND_ARG_INFO(0, name)
	ZEND_ARG_INFO(0, var)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemThread_get_args, ZEND_SEND_BY_VAL)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemThread_isset_args, ZEND_SEND_BY_VAL)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(WinSystemThread_unset_args, ZEND_SEND_BY_VAL)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

/* {{{ proto bool Win\System\Thread::start()
       want it to take any random callback, and data, and spawn it in a new thread*/
PHP_METHOD(WinSystemThread, start)
{
	HANDLE thread_handle;
	DWORD thread_id = 0;
	SECURITY_ATTRIBUTES thread_attributes;
	BOOL inherit = TRUE;

	winsystem_thread_data *thread_callback_data;

	zend_fcall_info finfo;
	zend_fcall_info_cache fcache;
	zval ***args = NULL;
	int argc = 0;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, ce_winsystem_argexception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "f*", &finfo, &fcache, &args, &argc) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	thread_callback_data = pecalloc(1, sizeof(winsystem_thread_data), 1);
	zend_fcall_info_argp(&finfo TSRMLS_CC, argc, args);
	thread_callback_data->callback_info = finfo;
	thread_callback_data->parent_tsrmls = TSRMLS_C;
	thread_callback_data->file = pestrdup(SG(request_info).path_translated, 1);
	thread_callback_data->callback_info.retval_ptr_ptr = NULL;
	thread_callback_data->callback_info.function_table = NULL;
	thread_callback_data->callback_info.symbol_table = NULL;

	thread_attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	thread_attributes.lpSecurityDescriptor = NULL;
	thread_attributes.bInheritHandle = inherit;

	thread_callback_data->start_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (thread_callback_data->start_event == NULL) {
		winsystem_create_error(GetLastError(), ce_winsystem_exception TSRMLS_CC);
		return;
	}

	/* Create the thread and store the handle in our new object */
	thread_handle = (HANDLE) _beginthreadex( 
            &thread_attributes,                   
            0,                      
            php_winsystem_thread_callback,       
            thread_callback_data, 
            0,
            &thread_id);

	/* Wait for the "event start" to be done in the new thread*/
	WaitForSingleObject(thread_callback_data->start_event, INFINITE);
	CloseHandle(thread_callback_data->start_event);

	/* This is a per thread list that holds all child threads spawned
	   Threads will be waited on in mshutdown */ 
	thread_callback_data->thread_handle = thread_handle;
	thread_callback_data->thread_id = thread_id;
	zend_llist_add_element(&WINSYSTEM_G(threads), (void *)thread_callback_data);

	/* TODO: create our thread object and send it back, with the thread handle and id inside */
}
/* }}} */

/* {{{ proto bool Win\System\Thread::set()
       final static method that will place data into a global data store
	   so it can be shared between threads - this will SERIALIZE any non-scalar data
	   coming in */
PHP_METHOD(WinSystemThread, set)
{
	char *var_name;
	unsigned int var_name_length;
	smart_str new_var = {0};
	char *serialized_string = NULL;
	php_serialize_data_t var_hash;
	zval *var;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &var_name, &var_name_length, &var) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	} 
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	/* serialize variables that will be shared */
	PHP_VAR_SERIALIZE_INIT(var_hash);
	php_var_serialize(&new_var, &var, &var_hash TSRMLS_CC);
	PHP_VAR_SERIALIZE_DESTROY(var_hash);

	/* copy the serialized string */
	serialized_string = estrndup(new_var.c, new_var.len);

	/* stick the string into the hash */
	if (SUCCESS == zend_ts_hash_update(&winsystem_threads_globals, var_name, var_name_length + 1, (void *) serialized_string, new_var.len + 1, NULL)) {
		RETVAL_FALSE
	} else {
		RETVAL_FALSE
	}
	smart_str_free(&new_var); 
}
/* }}} */

/* {{{ proto bool Win\System\Thread::get()
       grabs and deserializes data put in the global data store */
PHP_METHOD(WinSystemThread, get)
{
	char *var_name;
	unsigned int var_name_length;
	char * new_var = NULL;
	php_unserialize_data_t var_hash;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &var_name, &var_name_length) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	} 
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	/* grab the string from the hash */
	if (FAILURE == zend_ts_hash_find(&winsystem_threads_globals, var_name, var_name_length + 1, (void **) &new_var)) {
		return;
	}

	PHP_VAR_UNSERIALIZE_INIT(var_hash);

	php_var_unserialize(&return_value, (const unsigned char**)&new_var, new_var + strlen(new_var), &var_hash TSRMLS_CC);

	PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
}
/* }}} */

/* {{{ proto bool Win\System\Thread::isset()
       checks to see if a value exists in the global data store */
PHP_METHOD(WinSystemThread, isset)
{
	char *var_name;
	unsigned int var_name_length;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &var_name, &var_name_length) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	} 
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	/* do zend_hash_exists */
	RETURN_BOOL(zend_ts_hash_exists(&winsystem_threads_globals, var_name, var_name_length + 1));
}
/* }}} */

/* {{{ proto bool Win\System\Thread::unset()
       removes a value from the global data store */
PHP_METHOD(WinSystemThread, unset)
{
	char *var_name;
	unsigned int var_name_length;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &var_name, &var_name_length) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	} 
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	/* delete it */
	RETURN_BOOL(zend_ts_hash_del(&winsystem_threads_globals, var_name, var_name_length + 1));
}
/* }}} */

/* register thread methods */
static zend_function_entry winsystem_thread_functions[] = {
	// PHP_ME(WinSystemThread, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL | ZEND_ACC_CTOR)
	PHP_ME(WinSystemThread, start, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemThread, set, WinSystemThread_set_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemThread, get, WinSystemThread_get_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemThread, isset, WinSystemThread_isset_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemThread, unset, WinSystemThread_unset_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemWaitable, wait, WinSystemWaitable_wait_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, waitMsg, WinSystemWaitable_waitMsg_args, ZEND_ACC_PUBLIC)
	PHP_ME(WinSystemWaitable, signalAndWait, WinSystemWaitable_signalAndWait_args, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Thread C API                                                    
------------------------------------------------------------------*/

zend_object_value php_winsystem_thread_clone(zval *zobject, void ***prev_tsrm_ls TSRMLS_DC)
{
	zend_object_value new_obj_val;
	zend_object *old_object;
	zend_object *new_object;
	zend_object_handle handle = Z_OBJ_HANDLE_P(zobject);

	/* assume that create isn't overwritten, so when clone depends on the 
	 * overwritten one then it must itself be overwritten */
	old_object = zend_objects_get_address(zobject, prev_tsrm_ls);
	new_obj_val = zend_objects_new(&new_object, old_object->ce TSRMLS_CC);

	ALLOC_HASHTABLE(new_object->properties);
	zend_hash_init(new_object->properties, 0, NULL, ZVAL_PTR_DTOR, 0);

	zend_objects_clone_members(new_object, new_obj_val, old_object, handle TSRMLS_CC);

	return new_obj_val;
}

/* {{{ php_winsystem_thread_copy_zval_in_hash() */
static HashTable *php_winsystem_thread_copy_zval_in_hash(HashTable *src,
		void ***prev_tsrm_ls TSRMLS_DC)
{
	HashTable *retval;
	HashPosition pos;
	zend_hash_key key;
	int key_type;

	ALLOC_HASHTABLE(retval);
	zend_hash_init(retval, 0, NULL, ZVAL_PTR_DTOR, 0);

	for (zend_hash_internal_pointer_reset_ex(src, &pos);
			HASH_KEY_NON_EXISTANT != (
				key_type = zend_hash_get_current_key_ex(src, &key.arKey,
					&key.nKeyLength, &key.h, 0, &pos));
			zend_hash_move_forward_ex(src, &pos)) {
		zval **orig;
		zval *val;
		zend_hash_get_current_data_ex(src, (void **)&orig, &pos);
		if (FAILURE == php_winsystem_thread_copy_zval(&val, *orig,
					prev_tsrm_ls TSRMLS_CC)) {
			zend_hash_destroy(retval);
			FREE_HASHTABLE(retval);
			return NULL;
		}
		zend_hash_quick_add(retval, key.arKey,
				key_type == HASH_KEY_IS_LONG ? 0: key.nKeyLength, key.h, &val,
				sizeof(zval*), NULL);
	}
	return retval;
}
/* }}} */

static int php_winsystem_thread_copy_zval(zval **retval, zval *src,
		void ***prev_tsrm_ls TSRMLS_DC)
{
	ALLOC_ZVAL(*retval);
	if (Z_ISREF_P(src)) {
		(*retval)->type = IS_NULL;
	} else {
		if (Z_TYPE_P(src) == IS_OBJECT) {
			zend_class_entry *ce;
			zend_function *clone;
			zend_object_clone_obj_t clone_call;

			if (!Z_OBJ_HT_P(src)->get_class_entry
					|| !Z_OBJ_HT_P(src)->get_properties) {
				goto fail;
			}
			ce = Z_OBJ_HT_P(src)->get_class_entry(src, prev_tsrm_ls);
			ce = zend_fetch_class(ce->name, ce->name_length, 0 TSRMLS_CC);
			if (ce == NULL) {
				goto fail;
			}
			clone = ce ? ce->clone : NULL;
			clone_call =  Z_OBJ_HT_P(src)->clone_obj;
			if (!clone_call) {
				goto fail;
			}
			if (clone_call == zend_objects_clone_obj) {
				Z_OBJVAL_PP(retval) = php_winsystem_thread_clone(src, prev_tsrm_ls TSRMLS_CC);
			} else {
				php_printf("Object type cannot be cloned cross-thread");
				goto fail;
			}
			Z_TYPE_PP(retval) = IS_OBJECT;

			


			/*zend_class_entry *ce;
			HashTable *props;
			
			props = Z_OBJ_HT_P(src)->get_properties(src, prev_tsrm_ls);
			if (!props) {
				goto fail;
			}
			props = php_winsystem_thread_copy_zval_in_hash(props,
					prev_tsrm_ls TSRMLS_CC);
			(*retval)->type = IS_OBJECT;
			if (FAILURE == object_and_properties_init(*retval, ce, props)) {
				goto fail;
			}*/

		} else if (src->type == IS_ARRAY) {
			(*retval)->type = IS_ARRAY;
			(*retval)->value.ht = php_winsystem_thread_copy_zval_in_hash(
					src->value.ht, prev_tsrm_ls TSRMLS_CC);
			if (!(*retval)->value.ht) {
				goto fail;
			}
		} else if (src->type == IS_RESOURCE) {
		} else {
			**retval = *src;
			zval_copy_ctor(*retval);
		}
	}
	Z_SET_REFCOUNT_P(*retval, 1);
	Z_UNSET_ISREF_P(*retval);
	return SUCCESS;
fail:
	FREE_ZVAL(*retval);
	return FAILURE;
}
/* }}} */
void zend_class_add_ref(zend_class_entry **ce)
{
	(*ce)->refcount++;
}

/* {{{ php_thread_executor_globals_reinit() */
static void php_thread_executor_globals_reinit(zend_executor_globals *dest,
		zend_executor_globals *src)
{
	dest->current_module = src->current_module;
}
/* }}} */

/* {{{ php_thread_compiler_globals_reinit() */
static void php_thread_compiler_globals_reinit(zend_compiler_globals *dest,
		zend_compiler_globals *src)
{
	zend_hash_clean(dest->function_table);
	zend_hash_copy(dest->function_table, src->function_table,
			(copy_ctor_func_t)function_add_ref, NULL,
			sizeof(zend_function));
	zend_hash_clean(dest->class_table);
	zend_hash_copy(dest->class_table, src->class_table,
			(copy_ctor_func_t)zend_class_add_ref, NULL,
			sizeof(zend_class_entry*));
	dest->last_static_member = 0;
}
/* }}} */

static DWORD WINAPI php_winsystem_thread_callback(LPVOID lpParam) 
{
	/* This will be the exit value for the thread */
	zval *retval = NULL;
	zend_uint i = 0;
	zval **args;

	/* This has to be done, because we need to set up new resources for the thread
	so it will have a different TSRMLS value anyway */
	TSRMLS_FETCH(); 

	winsystem_thread_data *thread_callback_data = (winsystem_thread_data *)lpParam;
	zend_fcall_info callable;

	callable.size = sizeof(callable);
	callable.params = NULL;
	callable.param_count = 0;
	callable.retval_ptr_ptr = NULL;
	callable.function_table = NULL;
	callable.symbol_table = NULL;
	callable.object_ptr = NULL;
	callable.no_separation = 1;
	callable.function_name = NULL;
    
	/* Just as if we were doing another whole PHP interpreter */
	php_request_startup(TSRMLS_C);

	php_thread_compiler_globals_reinit(
			(zend_compiler_globals*)(*tsrm_ls)[TSRM_UNSHUFFLE_RSRC_ID(compiler_globals_id)],
			(zend_compiler_globals*)(*(thread_callback_data->parent_tsrmls))[TSRM_UNSHUFFLE_RSRC_ID(compiler_globals_id)]);
	php_thread_executor_globals_reinit(
			(zend_executor_globals*)(*tsrm_ls)[TSRM_UNSHUFFLE_RSRC_ID(executor_globals_id)],
			(zend_executor_globals*)(*(thread_callback_data->parent_tsrmls))[TSRM_UNSHUFFLE_RSRC_ID(executor_globals_id)]);

	callable.function_table = EG(function_table);

	callable.params = safe_emalloc(thread_callback_data->callback_info.param_count, sizeof(zval*), 0);
	args = safe_emalloc(thread_callback_data->callback_info.param_count, sizeof(zval*), 0);
	for (i = 0; i < thread_callback_data->callback_info.param_count; ++i) {
		if (FAILURE == php_winsystem_thread_copy_zval(&args[i],
				*thread_callback_data->callback_info.params[i],
				thread_callback_data->parent_tsrmls TSRMLS_CC)) {
			SetEvent(thread_callback_data->start_event);
			goto out;
		}
		callable.params[i] = &args[i];
	}
	callable.param_count = thread_callback_data->callback_info.param_count;

	if (FAILURE == php_winsystem_thread_copy_zval(
				&callable.function_name,
				thread_callback_data->callback_info.function_name,
				thread_callback_data->parent_tsrmls TSRMLS_CC)) {
		SetEvent(thread_callback_data->start_event);
		goto out;
	}
	if (thread_callback_data->callback_info.object_ptr) {
		if (FAILURE == php_winsystem_thread_copy_zval(
				&callable.object_ptr,
				thread_callback_data->callback_info.object_ptr,
				thread_callback_data->parent_tsrmls TSRMLS_CC)) {
			SetEvent(thread_callback_data->start_event);
			goto out;
		}
	}

	/* Tell our main thread we've started */
	SetEvent(thread_callback_data->start_event);

	/* Do the actual callback */
	callable.retval_ptr_ptr = &retval;

	if (zend_call_function(&(callable), NULL TSRMLS_CC) == SUCCESS) {
		php_printf("we need to juggle to int and thread return value\n");
	} else {
		// TODO: thread exit code of EXCEPTION_ILLEGAL_INSTRUCTION 
	}

out:
	/* Cleanup, cleanup, everybody clean up... */
	for (i = 0; i < callable.param_count; ++i) {
		zval_ptr_dtor(&args[i]);
	}

	if (callable.function_name) {
		zval_ptr_dtor(&callable.function_name);
	}

	if (callable.object_ptr) {
		zval_ptr_dtor(&callable.object_ptr);
	}

	efree(callable.params);
	efree(args);

	if(retval) {
		zval_ptr_dtor(&retval);
	}

	zend_fcall_info_args_clear(&thread_callback_data->callback_info, 1);

	/* Request shutdown is going to destroy our llist of threads */
	php_request_shutdown(TSRMLS_C);

	_endthreadex(0);

    return 0;
} 

/* Simple function to take a child thread, wait for it to finish, then destroy the
   data we've stored for it */
static void winsystem_finish_thread(void *data)
{
	winsystem_thread_data *thread_data = (winsystem_thread_data *) data;

	WaitForSingleObject(thread_data->thread_handle, INFINITE);
	CloseHandle(thread_data->thread_handle);

	pefree(thread_data->file, 1);
	/* No need to pefree the thread_data, that is implicit */
}

/* ----------------------------------------------------------------
  Win\System\Thread LifeCycle Functions                                                    
------------------------------------------------------------------*/
PHP_MINIT_FUNCTION(winsystem_thread)
{
	zend_class_entry ce;
	
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Thread", winsystem_thread_functions);
	ce_winsystem_thread = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(ce_winsystem_thread TSRMLS_CC, 1, ce_winsystem_waitable);

	zend_ts_hash_init(&winsystem_threads_globals, 0, NULL, NULL, 1);

	return SUCCESS;
}


/* initialize the thread list
   this is per request, note that all items in callback
   have their OWN list */
PHP_RINIT_FUNCTION(winsystem_thread)
{
	zend_llist_init(&WINSYSTEM_G(threads), sizeof(winsystem_thread_data), winsystem_finish_thread, 1);
	return SUCCESS;
}

/* Free our thread stack for main process */
PHP_RSHUTDOWN_FUNCTION(winsystem_thread)
{
	zend_llist_destroy(&WINSYSTEM_G(threads)); 
	return SUCCESS;
}

/* free our thread globals storage */
PHP_MSHUTDOWN_FUNCTION(winsystem_thread)
{
	zend_ts_hash_destroy(&winsystem_threads_globals);
	return SUCCESS;
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
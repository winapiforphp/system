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

ZEND_DECLARE_MODULE_GLOBALS(winsystem);

#ifdef ZTS
#include "php_main.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/php_var.h" 

/* All the classes in this file */
zend_class_entry *ce_winsystem_thread;

/* Truly global storage - but also thread safe */
TsHashTable winsystem_threads_globals;

DWORD WINAPI winsystem_thread_callback(LPVOID lpParam);

/* ----------------------------------------------------------------
  Win\System\Thread Userland API                                                    
------------------------------------------------------------------*/

ZEND_BEGIN_ARG_INFO(WinSystemThread_run_args, ZEND_SEND_BY_VAL)
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

/* {{{ proto bool Win\System\Thread->__construct()
       runs the "run" method of the extended class */
PHP_METHOD(WinSystemThread, __construct)
{
	HANDLE thread_handle;
	DWORD thread_id = 0;
	zval ***args = NULL;
	int argc = 0, i = 0;
	zend_error_handling error_handling;
	winsystem_thread_data *thread_callback_data;
	winsystem_thread_object *handle_object = (winsystem_thread_object*)zend_object_store_get_object(getThis() TSRMLS_CC);
	
	zend_replace_error_handling(EH_THROW, ce_winsystem_exception, &error_handling TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	thread_callback_data = ecalloc(1, sizeof(winsystem_thread_data));
	thread_callback_data->src_filename = estrdup(zend_get_executed_filename(TSRMLS_C));
	thread_callback_data->src_lineno = zend_get_executed_lineno(TSRMLS_C);
	thread_callback_data->file = estrdup(SG(request_info).path_translated);
	thread_callback_data->parent_tsrmls = TSRMLS_C;
	thread_callback_data->classname = estrdup(Z_OBJCE_P(getThis())->name);
	thread_callback_data->classlen = Z_OBJCE_P(getThis())->name_length;

	/* Create an event for "we've started the thread" */
	thread_callback_data->start_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (thread_callback_data->start_event == NULL) {
		winsystem_create_error(GetLastError(), ce_winsystem_exception TSRMLS_CC);
		return;
	}

    /* Create the thread and store the handle in our new object */
	thread_handle = (HANDLE) _beginthreadex( 
            NULL,                   
            0,                      
            winsystem_thread_callback,       
            thread_callback_data, 
            0,
            &thread_id);

	/* Wait for the "event start" to be done in the new thread */
	WaitForSingleObject(thread_callback_data->start_event, INFINITE);
	CloseHandle(thread_callback_data->start_event);

	/* Store our shiny new thread */
	thread_callback_data->thread_handle = thread_handle;
	thread_callback_data->thread_id = thread_id;

	zend_llist_add_element(&WINSYSTEM_G(threads), (void *)thread_callback_data);
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
	PHP_ME(WinSystemThread, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_FINAL | ZEND_ACC_CTOR)
	PHP_ABSTRACT_ME(WinSystemThread, run, WinSystemThread_run_args)
	PHP_ME(WinSystemThread, set, WinSystemThread_set_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemThread, get, WinSystemThread_get_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemThread, isset, WinSystemThread_isset_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemThread, unset, WinSystemThread_unset_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* ----------------------------------------------------------------
  Win\System\Thread C API                                                    
------------------------------------------------------------------*/

DWORD WINAPI winsystem_thread_callback(LPVOID lpParam) 
{ 
    winsystem_thread_data *thread_callback_data;
	zend_file_handle file_handle;
	zend_op_array *orig_op_array = NULL;
	zend_op_array *new_op_array = NULL;

	/* This has to be done, because we need to set up new resources for the thread
	so it will have a different TSRMLS value anyway */
	TSRMLS_FETCH();

	/* Cast lpParam to something more useful */
    thread_callback_data = (winsystem_thread_data *)lpParam;
	thread_callback_data->child_tsrmls = TSRMLS_C;

    /* Just as if we were doing another whole PHP interpreter */
	php_request_startup(TSRMLS_C);

	file_handle.handle.fp = VCWD_FOPEN(thread_callback_data->file, "rb");
	file_handle.filename = thread_callback_data->file;
 	file_handle.type = ZEND_HANDLE_FP;
 	file_handle.opened_path = NULL;
 	file_handle.free_filename = 0;

 	EG(exit_status) = 0; 

	orig_op_array = EG(active_op_array);
	new_op_array= EG(active_op_array) = zend_compile_file(&file_handle, ZEND_INCLUDE TSRMLS_CC);
	zend_destroy_file_handle(&file_handle TSRMLS_CC);

	/* Tell our main thread we've started - warning, main thread is going to delete this event */
	SetEvent(thread_callback_data->start_event);

	if (EG(active_op_array)) {
		zend_class_entry **pce;
		zval fname, *thread, *retval = NULL;
		zend_fcall_info fci;

		zend_lookup_class(thread_callback_data->classname, thread_callback_data->classlen, &pce TSRMLS_CC);
		MAKE_STD_ZVAL(thread);
		object_init_ex(thread, *pce);

		ZVAL_STRINGL(&fname, "run", sizeof("run")-1, 0);

		fci.size = sizeof(fci);
		fci.function_table = &Z_OBJCE_P(thread)->function_table;
		fci.function_name = &fname;
		fci.symbol_table = NULL;
		fci.object_ptr = thread;
		fci.retval_ptr_ptr = &retval;
		fci.param_count = 0;
		fci.params = NULL;
		fci.no_separation = 1;

		zend_call_function(&fci, NULL TSRMLS_CC);
		
		destroy_op_array(EG(active_op_array) TSRMLS_CC);
		efree(EG(active_op_array));
		EG(active_op_array) = orig_op_array;
	}

	php_request_shutdown(NULL);

    return 0; 
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
    zend_hash_apply_with_arguments(&ce_winsystem_thread->function_table TSRMLS_CC, (apply_func_args_t) unset_abstract_flag, 1, ce_winsystem_waitable);
	ce_winsystem_thread->ce_flags &= ~ZEND_ACC_IMPLICIT_ABSTRACT_CLASS;

	zend_ts_hash_init(&winsystem_threads_globals, 0, NULL, NULL, 0);

	return SUCCESS;
}

/* free our critical section lock for writing the global vars */
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
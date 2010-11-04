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
#include "ext/spl/spl_exceptions.h"

ZEND_DECLARE_MODULE_GLOBALS(winsystem);

zend_class_entry *ce_winsystem_exception;
zend_class_entry *ce_winsystem_argexception;
zend_class_entry *ce_winsystem_versionexception;
zend_object_handlers winsystem_object_handlers;

/* {{{ winsystem_create_error - grabs a message from GetLastError int and throws an exception with it */
void winsystem_create_error(int error, zend_class_entry *ce TSRMLS_DC)
{
	DWORD ret;
	char * buffer = NULL;

	/* Get a system message from the getlasterror value, tell windows to allocate the buffer, and don't
	   sprintf in any args */
	ret = FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER |           
		FORMAT_MESSAGE_IGNORE_INSERTS |            
		FORMAT_MESSAGE_FROM_SYSTEM,                
		NULL,                       
		error,                                     
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&buffer,                           
		0,
		NULL);

	if (ret == 0) {
		zend_throw_exception(ce, "Cannot retrieve error message properly", GetLastError() TSRMLS_CC);
		return;
	}

	zend_throw_exception(ce, buffer, error TSRMLS_CC);
	LocalFree(buffer);
}
/* }}} */

/* {{{ wingui_juggle_type - takes a zval and juggles the type */
int winsystem_juggle_type(zval *value, int type TSRMLS_DC)
{
	zval tmp_value;

	switch(type)
	{
		case IS_BOOL:
			if (Z_TYPE_P(value) != IS_BOOL) {
				tmp_value = *value;
				zval_copy_ctor(&tmp_value);
				convert_to_boolean(&tmp_value);
				value = &tmp_value;
			}
			return SUCCESS;
		case IS_LONG:
			if (Z_TYPE_P(value) != IS_LONG) {
				tmp_value = *value;
				zval_copy_ctor(&tmp_value);
				convert_to_long(&tmp_value);
				value = &tmp_value;
			}
			return SUCCESS;
		case IS_STRING:
			if (Z_TYPE_P(value) != IS_STRING) {
				tmp_value = *value;
				zval_copy_ctor(&tmp_value);
				convert_to_string(&tmp_value);
				value = &tmp_value;
			}
			return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

zval *winsystem_object_read_property(zval *object, zval *member, int type TSRMLS_DC)
{
	zval *retval;
	winsystem_prop_handler *hnd = NULL;
	int worked = FAILURE;
	winsystem_generic_object *generic_object = (winsystem_generic_object*)zend_object_store_get_object(object TSRMLS_CC);

	MAKE_STD_ZVAL(retval);

	winsystem_juggle_type(member, IS_STRING TSRMLS_CC);
	if (generic_object->prop_handler != NULL) {
		worked = zend_hash_find(generic_object->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &hnd);
	}

	if (worked == SUCCESS) {
        if (hnd->read_func != NULL) {
            worked = hnd->read_func(generic_object, member, &retval TSRMLS_CC);
		    if (worked == SUCCESS) {
		    	Z_SET_REFCOUNT_P(retval, 0);
			    Z_UNSET_ISREF_P(retval);
		    } else {
		    	retval = EG(uninitialized_zval_ptr);
		    }
        } else {
            retval = std_object_handlers.read_property(object, member, 0 TSRMLS_CC);
        }
	} else {
		retval = std_object_handlers.read_property(object, member, 0 TSRMLS_CC);
	}

	return retval;
}

zval **winsystem_object_get_property_ptr_ptr(zval *object, zval *member TSRMLS_DC)
{
	zval**retval = NULL;
	winsystem_prop_handler *hnd;
	int worked = FAILURE;
	winsystem_generic_object *generic_object = (winsystem_generic_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (FAILURE == winsystem_juggle_type(member, IS_STRING TSRMLS_CC)) {
		return retval;
	}

	if (generic_object->prop_handler != NULL) {
		worked = zend_hash_find(generic_object->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &hnd);
	}
	if (worked == FAILURE) {
		retval = std_object_handlers.get_property_ptr_ptr(object, member TSRMLS_CC);
	}
	return retval;
}

void winsystem_object_write_property(zval *object, zval *member, zval *value TSRMLS_DC)
{
	winsystem_prop_handler *hnd = NULL;
	int worked = FAILURE;
	winsystem_generic_object *generic_object = (winsystem_generic_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (FAILURE == winsystem_juggle_type(member, IS_STRING TSRMLS_CC)) {
		return;
	}

	if (generic_object->prop_handler != NULL) {
		worked = zend_hash_find(generic_object->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &hnd);
	}

	if (worked == SUCCESS) {
		/* readonly properties will have NULL writers */
		if (hnd->write_func == NULL) {
			zend_throw_exception(ce_winsystem_exception, "Internal properties cannot be altered", 0 TSRMLS_CC);
		} else {
			hnd->write_func(generic_object, member, value TSRMLS_CC);
		}
	} else {
		std_object_handlers.write_property(object, member, value TSRMLS_CC);
	}
}

void winsystem_object_delete_property(zval *object, zval *member TSRMLS_DC)
{
	winsystem_prop_handler *hnd;
	int worked = FAILURE;
	winsystem_generic_object *generic_object = (winsystem_generic_object*)zend_object_store_get_object(object TSRMLS_CC);

	if (FAILURE == winsystem_juggle_type(member, IS_STRING TSRMLS_CC)) {
		return;
	}

	if (generic_object->prop_handler != NULL) {
		worked = zend_hash_find(generic_object->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &hnd);
	}

	if (worked == SUCCESS) {
		zend_throw_exception(ce_winsystem_exception, "Internal properties cannot be unset", 0 TSRMLS_CC);
	} else {
		std_object_handlers.unset_property(object, member TSRMLS_CC);
	}
}

int winsystem_object_property_exists(zval *object, zval *member, int check_empty TSRMLS_DC)
{
	winsystem_prop_handler *hnd = NULL;
	int worked = FAILURE, retval = 0;
	winsystem_generic_object *generic_object = (winsystem_generic_object*)zend_object_store_get_object(object TSRMLS_CC);

 	if (FAILURE == winsystem_juggle_type(member, IS_STRING TSRMLS_CC)) {
		return retval;
	}

	if (generic_object->prop_handler != NULL) {
		worked = zend_hash_find(generic_object->prop_handler, Z_STRVAL_P(member), Z_STRLEN_P(member)+1, (void **) &hnd);
	}

	if (worked == SUCCESS) {
		zval *temp;

		MAKE_STD_ZVAL(temp);
		if (check_empty == 2) {
			retval = 1;
		} else if(hnd->read_func(generic_object, member, &temp TSRMLS_CC) == SUCCESS) {
			if (check_empty == 1) {
				retval = zend_is_true(temp);
			} else {
				retval = (Z_TYPE_P(temp) != IS_NULL);
			}
		}
		zval_ptr_dtor(&temp);
	} else {
		retval = std_object_handlers.has_property(object, member, check_empty TSRMLS_CC);
	}
	return retval;
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(winsystem_util)
{
	zend_class_entry ce, arg_ce, version_ce;

	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Exception", NULL);
	ce_winsystem_exception = zend_register_internal_class_ex(&ce, zend_exception_get_default(TSRMLS_C), "Exception" TSRMLS_CC);

	INIT_NS_CLASS_ENTRY(arg_ce, PHP_WINSYSTEM_NS, "ArgumentException", NULL);
	ce_winsystem_argexception = zend_register_internal_class_ex(&arg_ce, spl_ce_InvalidArgumentException, "InvalidArgumentException" TSRMLS_CC);

	INIT_NS_CLASS_ENTRY(version_ce, PHP_WINSYSTEM_NS, "VersionException", NULL);
	ce_winsystem_versionexception = zend_register_internal_class_ex(&version_ce, spl_ce_RuntimeException, "RuntimeException" TSRMLS_CC);

    memcpy(&winsystem_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	winsystem_object_handlers.get_property_ptr_ptr = winsystem_object_get_property_ptr_ptr;
    winsystem_object_handlers.unset_property = winsystem_object_delete_property;
	winsystem_object_handlers.write_property = winsystem_object_write_property;
	winsystem_object_handlers.read_property  = winsystem_object_read_property;
	winsystem_object_handlers.has_property   = winsystem_object_property_exists;
	//winsystem_object_handlers.get_debug_info = winsystem_object_debug_info;
	winsystem_object_handlers.clone_obj      = NULL;

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
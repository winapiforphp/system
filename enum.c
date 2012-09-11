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
  | Author: Elizabeth M Smith <auroraeosrose@gmail.com>                  |
  +----------------------------------------------------------------------+
*/

#include "php_winsystem.h"

zend_class_entry *ce_winsystem_enum;
static zend_object_handlers winsystem_enum_handlers;

static int winsystem_enum_collect_elements(long *value TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key);
static int winsystem_enum_apply_set(long *option TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key);

struct _winsystem_enum_object {
	zend_object std;
	zend_bool   is_constructed;
	long        value;
	HashTable  *elements;
};

/* ----------------------------------------------------------------
    Win\System\Enum C API
------------------------------------------------------------------*/

/* {{{ exported function to take a zval** enum instance and give you back the long value */
PHP_WINSYSTEM_API long php_winsystem_get_enum_value(zval** enumclass TSRMLS_DC)
{
	winsystem_enum_object *enum_object;

	enum_object = (winsystem_enum_object *) zend_object_store_get_object(*enumclass TSRMLS_CC);
	return enum_object->value;
}
/* }}} */

/* {{{ exported function to take long and stick it in an enum class
        WARNING: This does NOT check values, so make sure you don't screw up */
PHP_WINSYSTEM_API void php_winsystem_set_enum_value(zval** enumclass, long value TSRMLS_DC)
{
	winsystem_enum_object *enum_object;

	enum_object = (winsystem_enum_object *) zend_object_store_get_object(*enumclass TSRMLS_CC);
	enum_object->value = value;
}
/* }}} */

/* ----------------------------------------------------------------
    Win\System\Enum class API
------------------------------------------------------------------*/

ZEND_BEGIN_ARG_INFO(Enum___construct_args, ZEND_SEND_BY_VAL)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(Enum_getName_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(Enum_getElements_args, ZEND_SEND_BY_VAL)
ZEND_END_ARG_INFO()

/* {{{ proto void Enum->__construct(mixed value)
                  constructor for enum class, string can be a special case
                  anything else is cast and we attempt to assign */
PHP_METHOD(Enum, __construct)
{
	winsystem_enum_object *enum_object;
	long *new_val;
	zend_bool found = FALSE;

	/* for zpp */
	long value;
	zval *cast = NULL;

	/* Valid - send a valid long, send a string of a set item, send any other item and it will
	 * be cast to a long and see if it's set */
	PHP_WINSYSTEM_EXCEPTIONS
	if (FAILURE == zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC,"l", &value)) {
		if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &cast)) {
			return;
		}
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	enum_object = (winsystem_enum_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	/* handle cast zval */
	if(NULL != cast) {
		if (Z_TYPE_P(cast) == IS_STRING &&
			SUCCESS == zend_hash_find(enum_object->elements, Z_STRVAL_P(cast), Z_STRLEN_P(cast) + 1, (void **)&new_val)) {
			enum_object->value = *new_val;
			return;
		}
		convert_to_long(cast);
		value = Z_LVAL_P(cast);
	}

	/* handle the "easy" case of a long */
	zend_hash_apply_with_arguments(enum_object->elements TSRMLS_CC, (apply_func_args_t)winsystem_enum_apply_set, 2, &value, &found TSRMLS_CC);

	if(found) {
		enum_object->value = value;
		return;
	}

	zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Value provided is not a const in enum %s", enum_object->std.ce->name);
}
/* }}} */

/* {{{ proto string Enum->getName()
get member name of current enum value */
PHP_METHOD(Enum, getName)
{
	winsystem_enum_object *enum_object;
	char *key;
	ulong pos;
	long *value;

	PHP_WINSYSTEM_EXCEPTIONS
	if (FAILURE == zend_parse_parameters_none()) {
		return;
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	enum_object = (winsystem_enum_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	zend_hash_internal_pointer_reset(enum_object->elements);

	while (zend_hash_get_current_key(enum_object->elements, &key, &pos, 0) == HASH_KEY_IS_STRING) {
		if (zend_hash_get_current_data(enum_object->elements, (void **)&value) == SUCCESS &&
			*value == enum_object->value) {
			RETURN_STRING(key, 1);
		}
		zend_hash_move_forward(enum_object->elements);
	}

}
/* }}} */

/* {{{ proto array Enum->getElements()
                   get array of available name => value pairs */
PHP_METHOD(Enum, getElements)
{
	winsystem_enum_object *enum_object;

	PHP_WINSYSTEM_EXCEPTIONS
	if (FAILURE == zend_parse_parameters_none()) {
		return;
	}
	PHP_WINSYSTEM_RESTORE_ERRORS

	enum_object = (winsystem_enum_object *) zend_object_store_get_object(getThis() TSRMLS_CC);

	array_init(return_value);
	zend_hash_apply_with_arguments(enum_object->elements TSRMLS_CC, (apply_func_args_t)winsystem_enum_collect_elements, 1, return_value);
}
/* }}} */

/* {{{ class methods */
static const zend_function_entry winsystem_enum_methods[] = {
	PHP_ME(Enum, __construct, Enum___construct_args, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Enum, getName, Enum_getName_args, ZEND_ACC_PUBLIC)
	PHP_ME(Enum, getElements, Enum_getElements_args, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
/* }}} */

/* ----------------------------------------------------------------
    Win\System\Enum Object management
------------------------------------------------------------------*/

/* {{{ winsystem_enum_collect_elements
       helper function for getElements call to collect all values */
static int winsystem_enum_collect_elements(long *value TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *return_value = va_arg(args, zval*);

	add_assoc_long(return_value, hash_key->arKey, *value);

	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ winsystem_enum_collect_constants */
static int winsystem_enum_collect_constants(zval **pzconst TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key)
{
	HashTable *elements = va_arg(args, HashTable*);
	char *classname = va_arg(args, char*);

	if(Z_TYPE_PP(pzconst) != IS_LONG) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Constant %s is being cast to an integer in Enum subclass %s", hash_key->arKey, classname);
		convert_to_long(*pzconst);
	}
	zend_hash_add(elements, hash_key->arKey, hash_key->nKeyLength, &Z_LVAL_PP(pzconst), sizeof(long), NULL);

	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ winsystem_enum_object_free */
static void winsystem_enum_object_free(void *object TSRMLS_DC)
{
	winsystem_enum_object *enum_object = (winsystem_enum_object *)object;

	zend_object_std_dtor(&enum_object->std TSRMLS_CC);
	enum_object->is_constructed = FALSE;
	enum_object->value = 0;
	zend_hash_destroy(enum_object->elements);
	FREE_HASHTABLE(enum_object->elements);

	efree(enum_object);
}
/* }}} */

/* {{{ winsystem_enum_object_create */
static zend_object_value winsystem_enum_object_create(zend_class_entry *ce TSRMLS_DC)
{
	zend_object_value retval;
	winsystem_enum_object *enum_object;

	enum_object = ecalloc(1, sizeof(winsystem_enum_object));
	zend_object_std_init((zend_object *) enum_object, ce TSRMLS_CC);
	enum_object->is_constructed = FALSE;
	enum_object->value = 0;
	ALLOC_HASHTABLE(enum_object->elements);
	zend_hash_init(enum_object->elements, 8, NULL, NULL, 0);

	object_properties_init(&enum_object->std, ce);

	zend_update_class_constants(ce TSRMLS_CC);

	zend_hash_apply_with_arguments(&ce->constants_table TSRMLS_CC, (apply_func_args_t)winsystem_enum_collect_constants, 2, enum_object->elements, enum_object->std.ce->name);

	retval.handle = zend_objects_store_put(enum_object,
		(zend_objects_store_dtor_t) zend_objects_destroy_object,
		(zend_objects_free_object_storage_t) winsystem_enum_object_free,
		NULL TSRMLS_CC);

	retval.handlers = &winsystem_enum_handlers;
	return retval;
}
/* }}} */

/* {{{ winsystem_enum_get */
static zval* winsystem_enum_get(zval *zobject TSRMLS_DC)
{
	winsystem_enum_object *enum_object = (winsystem_enum_object *) zend_object_store_get_object(zobject TSRMLS_CC);
	zval *value;

	MAKE_STD_ZVAL(value);
	ZVAL_LONG(value, enum_object->value);
	Z_SET_REFCOUNT_P(value, 0);

	return value;
} /* }}} */

/* {{{ winsystem_enum_apply_set */
static int winsystem_enum_apply_set(long *option TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key) 
{
	long *value = va_arg(args, long*);
	zend_bool *found = va_arg(args, zend_bool*);

	if(*value == *option) {
		*found = TRUE;
		return ZEND_HASH_APPLY_STOP;
	}

	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* {{{ winsystem_enum_set */
static void winsystem_enum_set(zval **zobject, zval *value TSRMLS_DC)
{
	winsystem_enum_object *enum_object = (winsystem_enum_object *) zend_object_store_get_object(*zobject TSRMLS_CC);
	zend_bool found = FALSE;
	long lvalue;
	zval *juggled;

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJ_HANDLER_P(value, get)) {
		juggled = Z_OBJ_HANDLER_P(value, get)(value TSRMLS_CC);
	} else {
		juggled = value;
	}

	/* if we have a string, do a zend_hash_find FIRST */
	if (Z_TYPE_P(juggled) == IS_STRING) {
		long *new_val;

		if (SUCCESS == zend_hash_find(enum_object->elements, Z_STRVAL_P(juggled), Z_STRLEN_P(juggled) + 1, (void **)&new_val)) {
			enum_object->value = *new_val;

			if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJ_HANDLER_P(value, get)) {
				zval_dtor(juggled);
				FREE_ZVAL(juggled);
			}
			return;
		}
	}

	/* If juggled is long or a string, leave them alone, otherwise juggle */
	if (Z_TYPE_P(juggled) != IS_LONG) {
		convert_to_long(juggled);
	}
	lvalue = Z_LVAL_P(juggled);

	zend_hash_apply_with_arguments(enum_object->elements TSRMLS_CC, (apply_func_args_t)winsystem_enum_apply_set, 2, &lvalue, &found TSRMLS_CC);

	if (!found) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC, "Value provided is not a const in enum %s", enum_object->std.ce->name);
	} else {
		enum_object->value = Z_LVAL_P(juggled);
	}

	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJ_HANDLER_P(value, get)) {
		zval_dtor(juggled);
		FREE_ZVAL(juggled);
	}
}
/* }}} */

/* {{{ winsystem_enum_cast */
static int winsystem_enum_cast(zval *readobj, zval *writeobj, int type TSRMLS_DC)
{
	winsystem_enum_object *enum_object = (winsystem_enum_object *) zend_object_store_get_object(readobj TSRMLS_CC);

	ZVAL_LONG(writeobj, enum_object->value);
	convert_to_explicit_type(writeobj, type);
	return SUCCESS;
}
/* }}} */

/* {{{ winsystem_enum_compare */
static int winsystem_enum_compare(zval *z1, zval *z2 TSRMLS_DC)
{
	if (Z_TYPE_P(z1) == IS_OBJECT && Z_TYPE_P(z2) == IS_OBJECT &&
		instanceof_function(Z_OBJCE_P(z1), ce_winsystem_enum TSRMLS_CC) &&
		instanceof_function(Z_OBJCE_P(z2), ce_winsystem_enum TSRMLS_CC)) {

			winsystem_enum_object *enum1 = (winsystem_enum_object *) zend_object_store_get_object(z1 TSRMLS_CC);
			winsystem_enum_object *enum2 = (winsystem_enum_object *) zend_object_store_get_object(z2 TSRMLS_CC);

			return (enum1->value == enum2->value) ? 0 : ((enum1->value < enum2->value) ? -1 : 1);
		}

	return 1;
}
/* }}} */

/* {{{ winsystem_enum_clone */
static zend_object_value winsystem_enum_clone(zval *zobject TSRMLS_DC)
{
	zend_object_value retval;
	winsystem_enum_object *new_object;
	winsystem_enum_object *old_object = (winsystem_enum_object *) zend_object_store_get_object(zobject TSRMLS_CC);

	retval = winsystem_enum_object_create(old_object->std.ce TSRMLS_CC);
	new_object = (winsystem_enum_object *) zend_object_store_get_object_by_handle(retval.handle TSRMLS_CC);

	zend_objects_clone_members(&new_object->std, retval, &old_object->std, Z_OBJ_HANDLE_P(zobject) TSRMLS_CC);

	new_object->value = old_object->value;

	return retval;
}
/* }}} */

/* {{{ winsystem_enum_debug_info */
static HashTable* winsystem_enum_debug_info(zval *obj, int *is_temp TSRMLS_DC)
{
	HashTable *debug_info, *std_props;
	zval *elements, *value;
	winsystem_enum_object *enum_object = (winsystem_enum_object *) zend_object_store_get_object(obj TSRMLS_CC);

	ALLOC_HASHTABLE(debug_info);
	zend_hash_init(debug_info, 2, NULL, ZVAL_PTR_DTOR, 0);

	std_props = zend_std_get_properties(obj TSRMLS_CC);
	zend_hash_copy(debug_info, std_props, (copy_ctor_func_t)zval_add_ref, NULL, sizeof(zval*));

	MAKE_STD_ZVAL(elements);
	array_init(elements);
	zend_hash_apply_with_arguments(enum_object->elements TSRMLS_CC, (apply_func_args_t)winsystem_enum_collect_elements, 1, elements);
	zend_hash_update(debug_info, "__elements", sizeof("__elements"), (void*)&elements, sizeof(zval *), NULL);

	MAKE_STD_ZVAL(value);
	ZVAL_LONG(value, enum_object->value);
	zend_hash_update(debug_info, "__value", sizeof("__value"), (void*)&value, sizeof(zval *), NULL);

	*is_temp = 1;
	return debug_info;
}
/* }}} */

/* ----------------------------------------------------------------
    Win\System\Enum Registration
------------------------------------------------------------------*/

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(winsystem_enum)
{
	zend_class_entry ce;
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Enum", winsystem_enum_methods);
	ce_winsystem_enum = zend_register_internal_class(&ce TSRMLS_CC);
	ce_winsystem_enum->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

	ce_winsystem_enum->create_object = winsystem_enum_object_create;
	memcpy(&winsystem_enum_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	winsystem_enum_handlers.cast_object = winsystem_enum_cast;
	winsystem_enum_handlers.get_debug_info = winsystem_enum_debug_info;
	winsystem_enum_handlers.get = winsystem_enum_get;
	winsystem_enum_handlers.set = winsystem_enum_set;
	winsystem_enum_handlers.clone_obj = winsystem_enum_clone;
	winsystem_enum_handlers.compare_objects = winsystem_enum_compare;

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
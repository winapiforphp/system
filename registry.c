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
  | Author: Mark Skilbeck <markskilbeck@php.net>                         |
  | Author: Elizabeth Smith <auroraeosrose@gmail.com>                    |
  +----------------------------------------------------------------------+
*/

#include "php_winsystem.h"

zend_class_entry *ce_winsystem_registry;

/* ----------------------------------------------------------------
  Win\System\Registry Userland API
------------------------------------------------------------------*/

ZEND_BEGIN_ARG_INFO_EX(WinSystemRegistry_getValue_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VAL, 2)
	ZEND_ARG_INFO(0, key_name)
	ZEND_ARG_INFO(0, value_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(WinSystemRegistry_setValue_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VAL, 3)
	ZEND_ARG_INFO(0, key_name)
	ZEND_ARG_INFO(0, value_name)
	ZEND_ARG_INFO(0, value_data)
	ZEND_ARG_INFO(0, value_type)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(WinSystemRegistry_disablePredefinedCache_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VAL, 0)
	ZEND_ARG_INFO(0, all)
ZEND_END_ARG_INFO()

/* {{{ proto mixed Win\System\Registry::getValue(string key_name, string value_name)
       Retrieves a value for the specified key and value from the registry.
       - key_name should be a path to the key, beginning with one of the valid root-keys specified
         here (http://msdn.microsoft.com/en-us/library/ms724868(v=VS.85).aspx).
       - value_name is the name of the value you wish to retrieve, and can be an empty string
*/
PHP_METHOD(WinSystemRegistry, getValue)
{
	HKEY key;
	char *value, 
		 *key_full,   /* Full key path, ex:   "HKEY_CURRENT_USER\Control Panel\Colors" */
		 *key_root,   /* Root of key, ex:     "HKEY_CURRENT_USER" */
		 *key_subkey; /* Sub key of root, ex: "Control Panel\Colors" */
	VOID *data;
	int value_len, key_full_len;
	DWORD size, type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss",
		&key_full, &key_full_len, &value, &value_len) == FAILURE) {
		return;
	}

	/* No separator character, so we just have a (hopefully valid) root key */
	if (!strchr(key_full, '\\')) {
		key_root   = key_full;
		key_subkey = NULL;
	} else {
		key_root   = strtok(key_full, "\\"); /* Until first backslash */
		key_subkey = strtok(NULL, "\0");     /* Then to end of string */
	}

	/* Now determine whether the key */
	if (strcmp(key_root, "HKEY_CLASSES_ROOT") == 0) {
		key = HKEY_CLASSES_ROOT;
	} else if (strcmp(key_root, "HKEY_CURRENT_CONFIG") == 0) {
		key = HKEY_CURRENT_CONFIG;
	} else if (strcmp(key_root, "HKEY_CURRENT_USER") == 0 ) {
		key = HKEY_CURRENT_USER;
	} else if (strcmp(key_root, "HKEY_LOCAL_MACHINE") == 0) {
		key = HKEY_LOCAL_MACHINE;
	} else if (strcmp(key_root, "HKEY_PERFORMANCE_DATA") == 0) {
		key = HKEY_PERFORMANCE_DATA;
	} else if (strcmp(key_root, "HKEY_PERFORMANCE_NLSTEXT") == 0) {
		key = HKEY_PERFORMANCE_NLSTEXT;
	} else if (strcmp(key_root, "HKEY_PERFORMANCE_TEXT") == 0) {
		key = HKEY_PERFORMANCE_TEXT;
	} else if (strcmp(key_root, "HKEY_USERS") == 0) {
		key = HKEY_USERS;
	} else {
		zend_throw_exception_ex(ce_winsystem_runtimeexception, 0 TSRMLS_CC, "%s is not a valid root key", key_root);
		return;
	}

	if (RegGetValue(key, key_subkey, value,
		RRF_RT_ANY | RRF_NOEXPAND, &type, NULL, &size) != ERROR_SUCCESS) {
		winsystem_create_error(GetLastError(), ce_winsystem_runtimeexception TSRMLS_CC);
		return;
	}

	data = malloc(size);

	if (RegGetValue(key, key_subkey, value, RRF_RT_ANY | RRF_NOEXPAND, &type, (PVOID) data, &size) != ERROR_SUCCESS)
	{
		winsystem_create_error(GetLastError(), ce_winsystem_runtimeexception TSRMLS_CC);
		return;
	}

	// This looks ugly. I don't like ugly.
	switch (type)
	{
		// 64 bit - return as string
		case REG_QWORD:
		{
			// 20, I think, should be enough as a 64 bit number has at most 20 places... right?
			char loveyoulonglongtime[20];
			sprintf(loveyoulonglongtime, "%lld", (unsigned long long) data);
			RETURN_STRING(loveyoulonglongtime, 1);

			break;
		}

		case REG_MULTI_SZ:
		{
			char *str = (char *) data;

			array_init(return_value);

			while (*str)
			{
				add_next_index_string(return_value, str, 1);
				str = str + strlen(str) + 1;
			}
			return;
			break;
		}

		case REG_DWORD_BIG_ENDIAN:
		case REG_DWORD:
		{
			unsigned long *num = (unsigned long *) data;
			RETURN_LONG(*num);
			break;
		}

		case REG_SZ:
		case REG_BINARY:
		case REG_EXPAND_SZ:
		case REG_LINK:
			RETURN_STRINGL((char *) data, type == REG_BINARY ? size : size - 1, 1);

			break;

		default:
			/* What to do? */
			break;
	}
}
/* }}} */



/* {{{ proto void Win\System\Registry::setValue(string key_name, string value_name, mixed value_data[, int value_type])
       Sets data for the given registry key. If the key doesn't exist, it is created.
*/
PHP_METHOD(WinSystemRegistry, setValue)
{
	HKEY key, key_opened;
	char *value, 
		 *key_full,   // Full key path, ex:   "HKEY_CURRENT_USER\Control Panel\Colors"
		 *key_root,   // Root of key, ex:     "HKEY_CURRENT_USER"
		 *key_subkey; // Sub key of root, ex: "Control Panel\Colors" 
	zval *data_zval, *value_type_zval = NULL;

	int value_len, key_full_len, value_type;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ssz|z", &key_full, &key_full_len, 
		&value, &value_len, &data_zval, &value_type_zval) == FAILURE) {
		return;
	}

	// Using a zval for last param so we can check for a NULL value later on
	if (value_type_zval && Z_TYPE_P(value_type_zval) != IS_LONG) {
		zend_throw_exception_ex(ce_winsystem_runtimeexception, 0 TSRMLS_CC, "expected long for parameter 4, got %s", 
			zend_zval_type_name(value_type_zval));
		return;
	}

	// No separator character, so we just have a (hopefully valid) root key
	if ( ! strchr(key_full, '\\') ) {
		key_root   = key_full;
		key_subkey = NULL;
	} else {
		key_root   = strtok(key_full, "\\"); // Until first backslash
		key_subkey = strtok(NULL, "\0");     // Then to end of string
	}

	/* Now determine whether the key */
	if (strcmp(key_root, "HKEY_CLASSES_ROOT") == 0) {
		key = HKEY_CLASSES_ROOT;
	} else if (strcmp(key_root, "HKEY_CURRENT_CONFIG") == 0) {
		key = HKEY_CURRENT_CONFIG;
	} else if (strcmp(key_root, "HKEY_CURRENT_USER") == 0 ) {
		key = HKEY_CURRENT_USER;
	} else if (strcmp(key_root, "HKEY_LOCAL_MACHINE") == 0) {
		key = HKEY_LOCAL_MACHINE;
	} else if (strcmp(key_root, "HKEY_PERFORMANCE_DATA") == 0) {
		key = HKEY_PERFORMANCE_DATA;
	} else if (strcmp(key_root, "HKEY_PERFORMANCE_NLSTEXT") == 0) {
		key = HKEY_PERFORMANCE_NLSTEXT;
	} else if (strcmp(key_root, "HKEY_PERFORMANCE_TEXT") == 0) {
		key = HKEY_PERFORMANCE_TEXT;
	} else if (strcmp(key_root, "HKEY_USERS") == 0) {
		key = HKEY_USERS;
	}
	else {
		zend_throw_exception_ex(ce_winsystem_outofboundsexception, 0 TSRMLS_CC, "%s is not a valid root key", key_root);
		return;
	}

	if (RegCreateKeyEx(key, key_subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key_opened, NULL) != ERROR_SUCCESS) {
		winsystem_create_error(GetLastError(), ce_winsystem_runtimeexception TSRMLS_CC);
		return;
	}

	/* Determine type from zval */
	if ( ! value_type_zval ) {
		/* Only know how to handle longs, strings and arrays of strings.
		   Any other type defaults to string. */
		if (Z_TYPE_P(data_zval) == IS_LONG) {
			value_type = REG_DWORD;
		} else if (Z_TYPE_P(data_zval) == IS_STRING) {
			value_type = REG_SZ;
		} else if (Z_TYPE_P(data_zval) == IS_ARRAY) {
			value_type = REG_MULTI_SZ;
		} else {
			value_type = REG_SZ;
		}
	} else {
		value_type = Z_LVAL_P(value_type_zval);
	}

	if (value_type == REG_BINARY) {
		if (Z_TYPE_P(data_zval) != IS_STRING) {
			zend_throw_exception_ex(ce_winsystem_runtimeexception, 0 TSRMLS_CC, "expected string for binary type, got %s",
				zend_zval_type_name(data_zval));
			return;
		}

		if (RegSetKeyValue(key_opened, NULL, value, value_type, 
						   (LPCVOID) Z_STRVAL_P(data_zval), Z_STRLEN_P(data_zval)) != ERROR_SUCCESS) {
			winsystem_create_error(GetLastError(), ce_winsystem_runtimeexception TSRMLS_CC);
			return;
		}
	} else if (value_type == REG_DWORD || value_type == REG_DWORD_BIG_ENDIAN || value_type == REG_DWORD_LITTLE_ENDIAN) {
		if (Z_TYPE_P(data_zval) != IS_LONG) {
			zend_throw_exception_ex(ce_winsystem_runtimeexception, 0 TSRMLS_CC, "expected long for dword type, got %s",
				zend_zval_type_name(data_zval));
			return;
		}

		if (RegSetKeyValue(key_opened, NULL, value, value_type, &(Z_LVAL_P(data_zval)), sizeof(DWORD)) != ERROR_SUCCESS) {
			winsystem_create_error(GetLastError(), ce_winsystem_runtimeexception TSRMLS_CC);
			return;
		}
	} else if (value_type == REG_SZ || value_type == REG_EXPAND_SZ || value_type == REG_LINK) {
		if (Z_TYPE_P(data_zval) != IS_STRING) {
			zend_throw_exception_ex(ce_winsystem_runtimeexception, 0 TSRMLS_CC, "expected string for string type, got %s",
				zend_zval_type_name(data_zval));
			return;
		}

		if (RegSetKeyValue(key_opened, NULL, value, value_type, (LPCVOID) Z_STRVAL_P(data_zval), 
				Z_STRLEN_P(data_zval)) != ERROR_SUCCESS) {
			winsystem_create_error(GetLastError(), ce_winsystem_runtimeexception TSRMLS_CC);
			return;
		}
	} else if (value_type == REG_MULTI_SZ) {
		HashTable *hash;
		HashPointer pointer;
		char *multi_string = NULL;
		int  offset = 0;

		if (Z_TYPE_P(data_zval) != IS_ARRAY) {
			zend_throw_exception_ex(ce_winsystem_runtimeexception, 0 TSRMLS_CC, "expected array for multi-string type, got %s",
				zend_zval_type_name(data_zval));
			return;
		}

		hash = Z_ARRVAL_P(data_zval);
		if (zend_hash_num_elements(hash) == 0) {
			zend_throw_exception(ce_winsystem_runtimeexception, "got an empty array", 0 TSRMLS_CC);
			return;
		}

		for (zend_hash_internal_pointer_reset(hash);
			 zend_hash_has_more_elements(hash) == SUCCESS;
			 zend_hash_move_forward(hash)) {
			zval **current_string;
			int string_len;

			/* Using a HashPointer, we can tell what array index we're at (0, 1, 2, ...),
			   instead of having to use a counter variable. */
			zend_hash_get_pointer(hash, &pointer);
			zend_hash_get_current_data(hash, (void **) &current_string);

			/* We want a string */
			if (Z_TYPE_PP(current_string) != IS_STRING) {
				zend_throw_exception_ex(ce_winsystem_runtimeexception, 0 TSRMLS_CC, "expected array of strings, got %d in array",
					zend_zval_type_name(*current_string));
				free(current_string);
				return;
			}

			/* + 1 includes the terminating NULL character */
			string_len   = Z_STRLEN_PP(current_string) + 1;
			multi_string = erealloc(multi_string, offset + string_len);
			memcpy(multi_string + offset, Z_STRVAL_PP(current_string), string_len);

			offset += string_len;
		}

		/* We need an extra NULL on the end of the string to terminate 
		   the mothership... mother string, I mean.*/
		multi_string = erealloc(multi_string, offset + 1);
		multi_string[offset] = '\0';

		if (RegSetKeyValue(key_opened, NULL, value, value_type, (LPCVOID) multi_string, offset) != ERROR_SUCCESS) {
			winsystem_create_error(GetLastError(), ce_winsystem_runtimeexception TSRMLS_CC);
			efree(multi_string);
			return;
		}

		efree(multi_string);
	} else if (value_type == REG_QWORD || value_type == REG_QWORD_LITTLE_ENDIAN) {
		/* TODO: Should we allow longs to be passed in? */
		if (Z_TYPE_P(data_zval) != IS_STRING) {
			zend_throw_exception_ex(ce_winsystem_runtimeexception, 0 TSRMLS_CC,
				"expected string for qword type, got %s", zend_zval_type_name(data_zval));
			return;
		}

		if (RegSetKeyValue(key_opened, NULL, value, value_type, (LPCVOID) Z_STRVAL_P(data_zval), 
			Z_STRLEN_P(data_zval)) != ERROR_SUCCESS) {
			winsystem_create_error(GetLastError(), ce_winsystem_runtimeexception TSRMLS_CC);
			return;
		}
	}
}

/* }}} */

/* {{{ proto array Registry::getSystemRegistryQuota()
       Returns an array with the 'used' and 'allowed' statistics for the registry
*/
PHP_METHOD(WinSystemRegistry, getSystemRegistryQuota)
{
	DWORD used, allowed;

	if (zend_parse_parameters_none() != SUCCESS) {
		return;
	}

	if ( ! GetSystemRegistryQuota(&allowed, &used)) {
		winsystem_create_error(GetLastError(), ce_winsystem_runtimeexception TSRMLS_CC);
		return;
	}

	array_init(return_value);
	add_assoc_long(return_value, "allowed", allowed);
	add_assoc_long(return_value, "used",    used);
}
/* }}} */

/* {{{ proto void Registry::disabledPredefinedCache(bool $all)
       Disables handle caching for HKEY_CURRENT_USER.
       If 'all' is true, disables caching for all root keys.
*/
PHP_METHOD(WinSystemRegistry, disablePredefinedCache)
{
	zend_bool all = FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|b", &all) == FAILURE) {
		return;
	}

	if (all) {
		if (RegDisablePredefinedCacheEx() != ERROR_SUCCESS) {
			winsystem_create_error(GetLastError(), ce_winsystem_runtimeexception TSRMLS_CC);
			return;
		}
	} else {
		if (RegDisablePredefinedCache() != ERROR_SUCCESS) {
			winsystem_create_error(GetLastError(), ce_winsystem_runtimeexception TSRMLS_CC);
			return;
		}
	}
}

zend_function_entry winsystem_registry_functions[] = {
	PHP_ME(WinSystemRegistry, getValue, WinSystemRegistry_getValue_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemRegistry, setValue, WinSystemRegistry_setValue_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemRegistry, getSystemRegistryQuota, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(WinSystemRegistry, disablePredefinedCache, WinSystemRegistry_disablePredefinedCache_args, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	{ NULL, NULL, NULL }
};

/* ----------------------------------------------------------------
  Win\System\Registry Object Lifecycle Functions
------------------------------------------------------------------*/

PHP_MINIT_FUNCTION(winsystem_registry)
{
	zend_class_entry ce;

	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Registry", winsystem_registry_functions);
	ce_winsystem_registry = zend_register_internal_class(&ce TSRMLS_CC);

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
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

ZEND_DECLARE_MODULE_GLOBALS(winsystem);

/* ----------------------------------------------------------------
  \Win\System LifeCycle Functions                                                    
------------------------------------------------------------------*/

PHP_MINIT_FUNCTION(winsystem)
{
	PHP_MINIT(winsystem_util)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(winsystem_unicode)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(winsystem_waitable)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(winsystem_mutex)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(winsystem_semaphore)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(winsystem_event)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(winsystem_timer)(INIT_FUNC_ARGS_PASSTHRU);
#ifdef ZTS
	//PHP_MINIT(winsystem_timerqueue)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(winsystem_thread)(INIT_FUNC_ARGS_PASSTHRU);
#endif
    PHP_MINIT(winsystem_registry)(INIT_FUNC_ARGS_PASSTHRU);
	return SUCCESS;
}

/* Free thread stuff if needed */
PHP_MSHUTDOWN_FUNCTION(winsystem)
{
#ifdef ZTS
	PHP_MSHUTDOWN(winsystem_thread)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
	ts_free_id(winsystem_globals_id);
#endif
	return SUCCESS;
}

/* initialize the thread list */
PHP_RINIT_FUNCTION(winsystem)
{
	zend_llist_init(&WINSYSTEM_G(processes), sizeof(HANDLE), NULL, 1);
#ifdef ZTS
	PHP_RINIT(winsystem_thread)(INIT_FUNC_ARGS_PASSTHRU);
#endif
	return SUCCESS;
}
/* }}} */ 

/* Free thread stuff if needed - will call the thread callbacks so we wait on them to finish */
PHP_RSHUTDOWN_FUNCTION(winsystem)
{
#ifdef ZTS
	PHP_RSHUTDOWN(winsystem_thread)(SHUTDOWN_FUNC_ARGS_PASSTHRU);
#endif
	zend_llist_destroy(&WINSYSTEM_G(processes)); 
	return SUCCESS;
}

/* Module entry for winsystem */
zend_module_entry winsystem_module_entry = {
	STANDARD_MODULE_HEADER,
	"winsystem",
	NULL,
	PHP_MINIT(winsystem),
	PHP_MSHUTDOWN(winsystem),
	PHP_RINIT(winsystem),
	PHP_RSHUTDOWN(winsystem),
	NULL,
	PHP_WINSYSTEM_VERSION,
	PHP_MODULE_GLOBALS(winsystem),
	NULL,
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};

/* Required for dynamic linking */
#ifdef COMPILE_DL_WINSYSTEM
ZEND_GET_MODULE(winsystem)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
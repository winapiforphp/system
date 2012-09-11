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

#ifndef IMPLEMENT_WAITABLE_H
#define IMPLEMENT_WAITABLE_H

ZEND_BEGIN_ARG_INFO_EX(WinSystemWaitable_wait_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, milliseconds)
	ZEND_ARG_INFO(0, alertable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(WinSystemWaitable_waitMsg_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
	ZEND_ARG_INFO(0, milliseconds)
	ZEND_ARG_INFO(0, mask)
	ZEND_ARG_INFO(0, alertable)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(WinSystemWaitable_signalAndWait_args, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, signal)
	ZEND_ARG_INFO(0, milliseconds)
	ZEND_ARG_INFO(0, alertable)
ZEND_END_ARG_INFO()

extern PHP_METHOD(WinSystemWaitable, wait);
extern PHP_METHOD(WinSystemWaitable, waitMsg);
extern PHP_METHOD(WinSystemWaitable, signalAndWait);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
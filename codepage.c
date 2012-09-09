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

zend_class_entry *ce_winsystem_codepage;

/* ----------------------------------------------------------------
    Win\System\Codepage Enum for codepage values
------------------------------------------------------------------*/

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(winsystem_codepage)
{
	zend_class_entry ce;
	INIT_NS_CLASS_ENTRY(ce, PHP_WINSYSTEM_NS, "Codepage", NULL);
	ce_winsystem_codepage = zend_register_internal_class_ex(&ce, ce_winsystem_enum, NULL TSRMLS_CC);

	REGISTER_ENUM_CONST("UTF16", 1200, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("UTF16BE", 1201, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("ANSI",        CP_ACP, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("THREAD_ANSI", CP_THREAD_ACP, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("OEM",         CP_OEMCP, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("MAC",         CP_MACCP, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("UTF7",        CP_UTF7, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("UTF8",        CP_UTF8, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("SYMBOL",      CP_SYMBOL, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("IBM037",  037, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM437",  437, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM500",  500, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ASMO708", 708, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("DOS720",  720, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IMB737",  737, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM775",  775, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM850",  850, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM852",  852, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM855",  855, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IMB857",  857, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM858",  858, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM860",  860, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM861",  861, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("DOS862",  862, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM863",  863, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM864",  864, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM865",  865, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("CP866",   866, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM869",  869, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM855",  855, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM870",  870, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("WINDOWS_874",    874, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("CP875",          875, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("SHIFT_JIS",      932, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("GB2312",         936, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("KS_C_5601_1987", 949, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("BIG5",           950, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("IBM01047", 1047, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM01140", 1140, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM01141", 1141, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM01142", 1142, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM01143", 1143, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM01144", 1144, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM01145", 1145, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM01146", 1146, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM01147", 1147, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM01148", 1148, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM01149", 1149, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("WINDOWS_1250", 1250, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("WINDOWS_1251", 1251, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("WINDOWS_1252", 1252, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("WINDOWS_1253", 1253, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("WINDOWS_1254", 1254, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("WINDOWS_1255", 1255, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("WINDOWS_1256", 1256, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("WINDOWS_1257", 1257, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("WINDOWS_1258", 1258, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("JOHAB", 1361, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("MAC",   10000, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("X_MAC_JAPANESE",    10001, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_CHINESETRAD", 10002, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_KOREAN",      10003, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_ARABIC",      10004, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_HEBREW",      10005, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_GREEK",       10006, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_CYRILLIC",    10007, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_CHINESESIMP", 10008, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_ROMANIAN",    10010, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_UKRAINIAN",   10017, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_THAI",        10021, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_CE",          10029, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_ICELANDIC",   10079, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_TURKISH",     10081, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_MAC_CROATION",    10082, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("X_CHINESE_CNS",  20000, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_CP20001",      20001, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_CHINESE_ETEN", 20002, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_CP20003",      20003, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_CP20004",      20004, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_CP20005",      20005, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("X_IA5",           20115, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_IA5_GERMAN",    20116, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_IA5_SWEDISH",   20117, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_IA5_NORWEGIAN", 20118, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("US_ASCII",        20127, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_CP20261",       20261, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_CP20269",       20269, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_EBCDIC_KOREAN", 20833, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM_THAI",        20838, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("KOI8_R",          20866, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("IBM273", 20273, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM277", 20277, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM278", 20278, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM280", 20280, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM284", 20284, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM285", 20285, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM290", 20290, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM297", 20297, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM420", 20420, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM423", 20423, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM424", 20424, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM871", 20871, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM880", 20880, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM905", 20905, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("IBM924", 20924, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("EUC_JP",         20932, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_CP20936",      20936, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_CP20949",      20949, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("CP1025",         21025, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("KOI8_U",         21866, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_8859_1",     28591, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_8859_2",     28592, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_8859_3",     28593, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_8859_4",     28594, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_8859_5",     28595, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_8859_6",     28596, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_8859_7",     28597, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_8859_8",     28598, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_8859_9",     28599, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_8859_13",    28603, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_8859_15",    28605, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_EUROPA",       29001, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_8859_8_I",   38598, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_2022_JP",    50220, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("CSISO2022JP",    50221, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_2022_JP_SO", 50222, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("ISO_2022_KR",    50225, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_CP50227",      50227, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("ISO_2022",      50229, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("EBCDIC_JP",     50930, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("EBCDIC_US",     50931, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("EBCDIC_KR",     50933, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("EBCDIC_CN_EX",  50935, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("EBCDIC_CN",     50936, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("EBCDIC_CN_TRA", 50937, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("EBCDIC_JP_EX",  50939, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("EUC_JP",        51932, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("EUC_CN",        51936, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("EUC_KR",        51949, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("EUC_CN_TRA",    51950, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("HZ_GB_2312",    52936, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("GB18030",       54936, ce_winsystem_codepage);

	REGISTER_ENUM_CONST("X_ISCII_DE", 57002, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_ISCII_BE", 57003, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_ISCII_TA", 57004, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_ISCII_TE", 57005, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_ISCII_AS", 57006, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_ISCII_OR", 57007, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_ISCII_KA", 57008, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_ISCII_MA", 57009, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_ISCII_GU", 57010, ce_winsystem_codepage);
	REGISTER_ENUM_CONST("X_ISCII_PA", 57011, ce_winsystem_codepage);

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
--TEST--
Win\System\Unicode->__construct();
--SKIPIF--
<?php
include __DIR__ . '/../skipif.inc';
?>
--FILE--
<?php
use Win\System\Unicode;
use Win\System\Codepage;

include __DIR__ . '/../strings.inc';

$unicode = new Unicode($ascii, new CodePage(CodePage::US_ASCII));
echo $unicode, PHP_EOL;
var_dump(strlen($unicode));

$unicode = new Unicode($utf8, new CodePage(CodePage::UTF8));
echo $unicode, PHP_EOL;
var_dump(strlen($unicode));

$unicode = new Unicode($utf16, new CodePage(CodePage::UTF16));
echo $unicode, PHP_EOL;
var_dump(strlen($unicode));

$unicode = new Unicode($utf16be, new CodePage(CodePage::UTF16BE));
echo $unicode, PHP_EOL;
var_dump(strlen($unicode));
?>
===DONE===
--EXPECT--
This is my string
int(17)
काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥
int(107)
日本語
int(9)
日本語
int(9)
===DONE===
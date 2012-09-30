--TEST--
Win\System\Unicode object get handler;
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Unicode;
use Win\System\Codepage;

include __DIR__ . '/../../strings.inc';

$unicode = new Unicode($ascii, new CodePage(CodePage::US_ASCII));
echo $unicode . ' foobar', PHP_EOL;

$unicode = new Unicode($utf8, new CodePage(CodePage::UTF8));
echo $unicode . ' foobar', PHP_EOL;

$unicode = new Unicode($utf16, new CodePage(CodePage::UTF16));
echo $unicode . ' foobar', PHP_EOL;

$unicode = new Unicode($utf16be, new CodePage(CodePage::UTF16BE));
echo $unicode . ' foobar', PHP_EOL;
?>
===DONE===
--EXPECTF--
This is my string foobar
काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥ foobar
日本語 foobar
日本語 foobar
===DONE===
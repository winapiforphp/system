--TEST--
Win\System\Unicode->__construct() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Unicode;

$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, Unicode::UTF_8);
$string_2 = file_get_contents(__DIR__ .'/utf16-data.txt');
$unicode = new Unicode($string_2, Unicode::UTF_16);
?>
--EXPECTF--
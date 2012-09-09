<?php
$ascii = 'This is my string';
$utf8 = file_get_contents(__DIR__ . '/utf8.txt');
$utf16 = substr(file_get_contents(__DIR__ . '/utf16.txt'), 2); /* Note we strip the LE BOM or you'll get weird things */
$utf16be = substr(file_get_contents(__DIR__ . '/utf16be.txt'), 2); /* Note we strip the LE BOM or you'll get weird things */
?>
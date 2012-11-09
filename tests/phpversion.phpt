--TEST--
phpversion('winsystem') version
--SKIPIF--
<?php
include __DIR__ . '/skipif.inc';
?>
--FILE--
<?php
var_dump(phpversion('winsystem'));
?>
===DONE===
--EXPECTF--
string(%d) "%s"
===DONE===
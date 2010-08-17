--TEST--
phpversion('winsystem') version
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
var_dump(phpversion('winsystem'));
?>
--EXPECTF--
string(%d) "%s"
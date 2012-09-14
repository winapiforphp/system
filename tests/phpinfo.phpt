--TEST--
phpinfo specific to winsystem
--SKIPIF--
<?php
include __DIR__ . '/skipif.inc';
?>
--FILE--
<?php
$ext = new ReflectionExtension('winsystem');
$ext->info();
?>
--EXPECTF--
string(%d) "Version => %s"
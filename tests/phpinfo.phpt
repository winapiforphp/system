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
winsystem

Version => %d.%d.%d%s
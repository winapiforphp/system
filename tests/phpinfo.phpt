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
===DONE===
--EXPECTF--
winsystem

Version => %d.%d.%d%s
===DONE===
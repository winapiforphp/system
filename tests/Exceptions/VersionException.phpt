--TEST--
Win\System\VersionException
--SKIPIF--
<?php
include __DIR__ . '/../skipif.inc';
?>
--FILE--
<?php
var_dump(class_exists('Win\System\VersionException'));

$e = new Win\System\VersionException();

// verify it extends base exception class
var_dump($e instanceof \Exception);
// verify it extends RuntimeException
var_dump($e instanceof \Win\System\RuntimeException);
// verify it implements Win\System\Exception
var_dump($e instanceof \Win\System\Exception);
?>
===DONE===
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
===DONE===
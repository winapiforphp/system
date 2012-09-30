--TEST--
Win\System\InvalidArgumentException
--SKIPIF--
<?php
include __DIR__ . '/../skipif.inc';
?>
--FILE--
<?php
var_dump(class_exists('Win\System\InvalidArgumentException'));

$e = new Win\System\InvalidArgumentException();

// verify it extends base exception class
var_dump($e instanceof \Exception);
// verify it extends InvalidArgumentException
var_dump($e instanceof \InvalidArgumentException);
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
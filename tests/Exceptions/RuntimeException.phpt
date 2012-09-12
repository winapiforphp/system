--TEST--
Win\System\RuntimeException
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
var_dump(class_exists('Win\System\RuntimeException'));

$e = new Win\System\RuntimeException();

// verify it extends base exception class
var_dump($e instanceof \Exception);
// verify it extends RuntimeException
var_dump($e instanceof \RuntimeException);
// verify it implements Win\System\Exception
var_dump($e instanceof \Win\System\Exception);
?>
= DONE =
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
= DONE =
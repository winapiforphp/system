--TEST--
Win\System\OutOfBoundsException
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
var_dump(class_exists('Win\System\OutOfBoundsException'));

$e = new Win\System\OutOfBoundsException();

// verify it extends base exception class
var_dump($e instanceof \Exception);
// verify it extends OutOfBoundsException
var_dump($e instanceof \Win\System\OutOfBoundsException);
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
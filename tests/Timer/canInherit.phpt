--TEST--
Win\System\Timer->canInherit() method
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Timer;
use Win\System\InvalidArgumentException;

// create a normal timer, default inherit is true
$timer = new Timer();
var_dump($timer->canInherit());

// create it explicitly off
$timer = new Timer(null, false, false);
var_dump($timer->canInherit());

// create it explicitly on
$timer = new Timer(null, false, true);
var_dump($timer->canInherit());

// bad number of args
try {
    $timer->canInherit(1);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
Win\System\Timer::canInherit() expects exactly 0 parameters, 1 given
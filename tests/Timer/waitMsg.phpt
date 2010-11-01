--TEST--
Win\System\Timer->waitMsg() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Timer;
use Win\System\WaitMask;
use Win\System\ArgumentException;

$timer = new Timer();
var_dump($timer->set(10));

var_dump($timer->waitMsg(30));
var_dump($timer->set(10));
var_dump($timer->waitMsg(30, WaitMask::ALLINPUT));
var_dump($timer->set(10));
var_dump($timer->waitMsg(30, WaitMask::ALLINPUT, true));

// no more then 3 args
try {
   var_dump($timer->waitMsg(1, 1, 1, 1));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be int
try {
    var_dump($timer->waitMsg(array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be int
try {
    var_dump($timer->waitMsg(1, array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be boolean
try {
    var_dump($timer->waitMsg(1, 1, array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
int(0)
bool(true)
int(0)
bool(true)
int(0)
Win\System\Timer::waitMsg() expects at most 3 parameters, 4 given
Win\System\Timer::waitMsg() expects parameter 1 to be long, array given
Win\System\Timer::waitMsg() expects parameter 2 to be long, array given
Win\System\Timer::waitMsg() expects parameter 3 to be boolean, array given
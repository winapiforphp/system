--TEST--
Win\System\Timer->wait() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Timer;
use Win\System\ArgumentException;

$timer = new Timer();
var_dump($timer->set(10));

var_dump($timer->wait());
var_dump($timer->set(10));
var_dump($timer->wait(30));
var_dump($timer->set(10));
var_dump($timer->wait(30, true));

// no more then 2 args
try {
   var_dump($timer->wait(1, 1, 1));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be int
try {
    var_dump($timer->wait(array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be boolean
try {
    var_dump($timer->wait(1, array()));
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
Win\System\Timer::wait() expects at most 2 parameters, 3 given
Win\System\Timer::wait() expects parameter 1 to be long, array given
Win\System\Timer::wait() expects parameter 2 to be boolean, array given
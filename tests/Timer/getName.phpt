--TEST--
Win\System\Timer->getName() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Timer;

// create an unnamed timer
$timer = new Timer();
var_dump($timer->getName());

// create a a timer with a null name
$timer = new Timer(null);
var_dump($timer->getName());

// create a timer with an empty string for a name
$timer = new Timer('');
var_dump($timer->getName());

// create a named timer
$timer = new Timer('foobar');
var_dump($timer->getName());

// bad number of args
try {
    $timer->getName(1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
NULL
NULL
string(0) ""
string(6) "foobar"
Win\System\Timer::getName() expects exactly 0 parameters, 1 given
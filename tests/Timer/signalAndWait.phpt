--TEST--
Win\System\Timer->signalAndWait() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Timer;
use Win\System\Event;
use Win\System\ArgumentException;

// create two timers
$timer = new Timer();
$timer->set(10);
$signal = new Event();

var_dump($timer->signalAndWait($signal));
$timer->set(10);
var_dump($timer->signalAndWait($signal, 30));
$timer->set(10);
var_dump($timer->signalAndWait($signal, 30, true));

// requires 1 arg
try {
   var_dump($timer->signalAndWait());
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// no more then 3 args
try {
   var_dump($timer->signalAndWait($signal, 1, 1, 1));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be waitable
try {
    var_dump($timer->signalAndWait(array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be int
try {
    var_dump($timer->signalAndWait($signal, array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be boolean
try {
    var_dump($timer->signalAndWait($signal, 1, array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(0)
int(0)
int(0)
Win\System\Timer::signalAndWait() expects at least 1 parameter, 0 given
Win\System\Timer::signalAndWait() expects at most 3 parameters, 4 given
Win\System\Timer::signalAndWait() expects parameter 1 to be Win\System\Waitable, array given
Win\System\Timer::signalAndWait() expects parameter 2 to be long, array given
Win\System\Timer::signalAndWait() expects parameter 3 to be boolean, array given
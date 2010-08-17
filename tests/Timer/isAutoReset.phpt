--TEST--
Win\System\Timer->isAutoReset() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Timer;

// create an unnamed timer
$timer = new Timer();

// default is autoreset on
var_dump($timer->isAutoReset());

// create an an unnamed timer with autoreset off
$timer = new Timer(null, false);

// now should be off
var_dump($timer->isAutoReset());

// bad number of args
try {
    $timer->isAutoReset(1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(false)
Win\System\Timer::isAutoReset() expects exactly 0 parameters, 1 given
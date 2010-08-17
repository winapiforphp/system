--TEST--
Win\System\Timer->cancel() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Timer;

// create a timer
$timer = new Timer();

// cancel the timer
var_dump($timer->cancel());

// bad number of args
try {
    $timer->cancel(1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
Win\System\Timer::cancel() expects exactly 0 parameters, 1 given
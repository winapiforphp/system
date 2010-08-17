--TEST--
Win\System\Mutex->canInherit() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Mutex;
use Win\System\Exception;

// create a normal mutex, default inherit is true
$mutex = new Mutex();
var_dump($mutex->canInherit());

// create it explicitly off
$mutex = new Mutex(null, false, false);
var_dump($mutex->canInherit());

// create it explicitly on
$mutex = new Mutex(null, false, true);
var_dump($mutex->canInherit());

// bad number of args
try {
    $mutex->canInherit(1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
Win\System\Mutex::canInherit() expects exactly 0 parameters, 1 given
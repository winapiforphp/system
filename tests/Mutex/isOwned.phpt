--TEST--
Win\System\Mutex->isOwned() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Mutex;
use Win\System\Exception;

// create an unnamed mutex
$mutex = new Mutex();

// we don't own it
var_dump($mutex->isOwned());

// create an owned unnamed mutex
$mutex = new Mutex(null, true);

// we do own it
var_dump($mutex->isOwned());

// create an unnamed mutex
$mutex = new Mutex();

// we don't own it
var_dump($mutex->isOwned());

// wait with a timeout, we should now own the mutex
$mutex->wait(1);

// we do own it
var_dump($mutex->isOwned());

// bad number of args
try {
    $mutex->isOwned(1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(true)
Win\System\Mutex::isOwned() expects exactly 0 parameters, 1 given
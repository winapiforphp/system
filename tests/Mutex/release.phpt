--TEST--
Win\System\Mutex->release() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Mutex;
use Win\System\ArgumentException;

// create an owned unnamed mutex
$mutex = new Mutex(null, true);

// we do own it
var_dump($mutex->isOwned());

// release it
var_dump($mutex->release());

// we don't own it
var_dump($mutex->isOwned());

// wait with a timeout, we should now own the mutex
$mutex->wait(1);

// we do own it
var_dump($mutex->isOwned());

// release it
var_dump($mutex->release());

// release it again
var_dump($mutex->release());

// we don't own it
var_dump($mutex->isOwned());

// bad number of args
try {
    $mutex->release(1);
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
Win\System\Mutex::release() expects exactly 0 parameters, 1 given
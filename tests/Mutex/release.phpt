--TEST--
Win\System\Mutex->release() method
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Mutex;
use Win\System\InvalidArgumentException;

// create an owned unnamed mutex
$mutex = new Mutex(null, true);

// release it
var_dump($mutex->release());

// wait with a timeout, we should now own the mutex
$mutex->wait(1);

// release it
var_dump($mutex->release());

// release it again fails
var_dump($mutex->release());

// bad number of args
try {
    $mutex->release(1);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
Win\System\Mutex::release() expects exactly 0 parameters, 1 given
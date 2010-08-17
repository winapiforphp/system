--TEST--
Win\System\Mutex->getName() method
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
var_dump($mutex->getName());

// create a a mutex with a null name
$mutex = new Mutex(null);
var_dump($mutex->getName());

// create a mutex with an empty string for a name
$mutex = new Mutex('');
var_dump($mutex->getName());

// create a named mutex
$mutex = new Mutex('foobar');
var_dump($mutex->getName());

// bad number of args
try {
    $mutex->getName(1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
NULL
NULL
string(0) ""
string(6) "foobar"
Win\System\Mutex::getName() expects exactly 0 parameters, 1 given
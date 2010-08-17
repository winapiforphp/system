--TEST--
Win\System\Semaphore->getName() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Semaphore;
use Win\System\Exception;

// create an unnamed semaphore
$semaphore = new Semaphore();
var_dump($semaphore->getName());

// create a a semaphore with a null name
$semaphore = new Semaphore(null);
var_dump($semaphore->getName());

// create a semaphore with an empty string for a name
$semaphore = new Semaphore('');
var_dump($semaphore->getName());

// create a named semaphore
$semaphore = new Semaphore('foobar');
var_dump($semaphore->getName());

// bad number of args
try {
    $semaphore->getName(1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
NULL
NULL
string(0) ""
string(6) "foobar"
Win\System\Semaphore::getName() expects exactly 0 parameters, 1 given
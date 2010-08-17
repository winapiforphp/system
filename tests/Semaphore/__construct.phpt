--TEST--
Win\System\Semaphore->__construct() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Semaphore;
use Win\System\Timer;

// new unnamed semaphore with default count and max count
$semaphore = new Semaphore();
var_dump($semaphore->getName());
var_dump($semaphore->getCount());
var_dump($semaphore->getMaxCount());

// new named semaphore with default count and max count
$semaphore = new Semaphore('foobar');
var_dump($semaphore->getName());
var_dump($semaphore->getCount());
var_dump($semaphore->getMaxCount());

// try to open timer with same name
try {
    $timer = new Timer('foobar');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// new semaphore with count and max count set
$semaphore = new Semaphore(null, 2, 5);
var_dump($semaphore->getName());
var_dump($semaphore->getCount());
var_dump($semaphore->getMaxCount());

// new semaphore that can inherit
$semaphore = new Semaphore('sem', 2, 5, true);
var_dump($semaphore->getName());
var_dump($semaphore->getCount());
var_dump($semaphore->getMaxCount());
var_dump($semaphore->canInherit());

// requires 0-4 args, 5 is too many
try {
    $semaphore = new Semaphore(1, 1, 1, 1, 1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be stringable
try {
    $semaphore = new Semaphore(array(), 1, 1, 1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be longable
try {
    $semaphore = new Semaphore('string', array(), 1, 1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be longable
try {
    $semaphore = new Semaphore('string', 1, array(), 1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg 4 must be booleanable
try {
    $semaphore = new Semaphore('string', 1, 1, array());
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be > 0
try {
    $semaphore = new Semaphore('string', -1, 1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be > 1
try {
    $semaphore = new Semaphore('string', 0, 0);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be > arg 2
try {
    $semaphore = new Semaphore('string', 10, 3);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
NULL
int(0)
int(1)
string(6) "foobar"
int(0)
int(1)
Could not create or open the requested timer
NULL
int(2)
int(5)
string(3) "sem"
int(2)
int(5)
bool(true)
Win\System\Semaphore::__construct() expects at most 4 parameters, 5 given
Win\System\Semaphore::__construct() expects parameter 1 to be string, array given
Win\System\Semaphore::__construct() expects parameter 2 to be long, array given
Win\System\Semaphore::__construct() expects parameter 3 to be long, array given
Win\System\Semaphore::__construct() expects parameter 4 to be boolean, array given
Count must be less than the max_count and 0 or greater
Max count must be at least 1
Count must be less than the max_count and 0 or greater
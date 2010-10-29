--TEST--
Win\System\Semaphore->wait() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Semaphore;
use Win\System\ArgumentException;

$semaphore = new Semaphore();

// a semaphore will wait forever without a timeout - ick
var_dump($semaphore->wait(30));
var_dump($semaphore->wait(30, true));

// no more then 2 args
try {
   var_dump($semaphore->wait(1, 1, 1));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be int
try {
    var_dump($semaphore->wait(array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be boolean
try {
    var_dump($semaphore->wait(1, array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(258)
int(258)
Win\System\Semaphore::wait() expects at most 2 parameters, 3 given
Win\System\Semaphore::wait() expects parameter 1 to be long, array given
Win\System\Semaphore::wait() expects parameter 2 to be boolean, array given
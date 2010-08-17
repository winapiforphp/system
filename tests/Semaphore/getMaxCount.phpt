--TEST--
Win\System\Semaphore->getMaxCount() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Semaphore;
use Win\System\Exception;

// create semaphore - default max count is 1
$semaphore = new Semaphore(null, 0);
var_dump($semaphore->getMaxCount());

// create a semaphore with a different maxcount
$semaphore = new Semaphore(null, 0, 10);
var_dump($semaphore->getMaxCount());

// bad number of args
try {
    $semaphore->getMaxCount(1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(1)
int(10)
Win\System\Semaphore::getMaxCount() expects exactly 0 parameters, 1 given
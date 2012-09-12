--TEST--
Win\System\Semaphore->getMaxCount() method
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Semaphore;
use Win\System\InvalidArgumentException;
use Win\System\RuntimeException;

// create semaphore - default max count is 1
$semaphore = new Semaphore(null, 0);
var_dump($semaphore->getMaxCount());

// create a semaphore with a different maxcount
$semaphore = new Semaphore(null, 0, 10);
var_dump($semaphore->getMaxCount());

// when you open a semaphore, the max count is unknown
$semaphore = new Semaphore('foobar', 10, 10);
$semaphore = Semaphore::open('foobar');

// bad number of args
try {
    $semaphore->getMaxCount();
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}

// bad number of args
try {
    $semaphore->getMaxCount(1);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(1)
int(10)
Max semaphore count unknown
Win\System\Semaphore::getMaxCount() expects exactly 0 parameters, 1 given
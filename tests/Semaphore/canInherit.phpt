--TEST--
Win\System\Semaphore->canInherit() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Semaphore;
use Win\System\Exception;

// create a normal mutex, default inherit is true
$semaphore = new Semaphore();
var_dump($semaphore->canInherit());

// create it explicitly off
$semaphore = new Semaphore(null, 0, 1, false);
var_dump($semaphore->canInherit());

// create it explicitly on
$semaphore = new Semaphore(null, 0, 1, true);
var_dump($semaphore->canInherit());

// bad number of args
try {
    $semaphore->canInherit(1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
Win\System\Semaphore::canInherit() expects exactly 0 parameters, 1 given
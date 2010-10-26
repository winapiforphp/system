--TEST--
Win\System\Semaphore->release() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Semaphore;
use Win\System\ArgumentException;

// create a default semaphore
$semaphore = new Semaphore();

// release it, adds one
var_dump($semaphore->release());

// cant' increase it again, it only goes up to the maximum
try {
    $semaphore->release();
} catch (Exception $e) {
   echo $e->getMessage(), "\n";
}

// decrease it with wait
$semaphore->wait(1);

// create a default semaphore
$semaphore = new Semaphore(null, 2, 10);

// now we can release it again
var_dump($semaphore->release());

// maximum is 1 arg
try {
    $semaphore->release(1, 1);
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg must be longable
try {
    $semaphore->release(array());
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(0)
Too many posts were made to a semaphore.

int(2)
Win\System\Semaphore::release() expects at most 1 parameter, 2 given
Win\System\Semaphore::release() expects parameter 1 to be long, array given
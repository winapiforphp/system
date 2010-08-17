--TEST--
Win\System\Semaphore->release() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Semaphore;
use Win\System\Exception;

// create a default semaphore
$semaphore = new Semaphore();

// default count is 0
var_dump($semaphore->getCount());

// release it, adds one
var_dump($semaphore->release());

// count is now 1
var_dump($semaphore->getCount());

// cant' increase it again, it only goes up to the maximum
try {
    $semaphore->release();
} catch (Exception $e) {
   echo $e->getMessage(), "\n";
}

// decrease it with wait
$semaphore->wait(1);

// create a default semaphore
$semaphore = new Semaphore(null, 0, 10);

// now we can release it again
var_dump($semaphore->release());

// maximum is 1 arg
try {
    $semaphore->release(1, 1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg must be longable
try {
    $semaphore->release(array());
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(0)
bool(true)
int(1)
Too many posts were made to a semaphore.

bool(true)
Win\System\Semaphore::release() expects at most 1 parameter, 2 given
Win\System\Semaphore::release() expects parameter 1 to be long, array given
--TEST--
Win\System\Semaphore->getCount() method
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

// default getCount is 0
var_dump($semaphore->getCount());

// release increases it by 1
var_dump($semaphore->release());

// count is now 1
var_dump($semaphore->getCount());

// create an unnamed semaphore with a max count of 10 that starts at 4
$semaphore = new Semaphore(null, 4, 10);

// count is 4
var_dump($semaphore->getCount());

// release increases it by 4
var_dump($semaphore->release(4));

// count is 8
var_dump($semaphore->getCount());

// cant' increase it again, it only goes up to the maximum
try {
    $semaphore->release(4);
} catch (Exception $e) {
   echo $e->getMessage(), "\n";
}

// decrease it with a wait
var_dump($semaphore->wait(0));

// count is 7
var_dump($semaphore->getCount());

// increate it to the maximum
var_dump($semaphore->release(3));

// now we're at 10, the end
var_dump($semaphore->getCount());

// bad number of args
try {
    $semaphore->getCount(1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(0)
bool(true)
int(1)
int(4)
bool(true)
int(8)
Too many posts were made to a semaphore.

int(0)
int(7)
bool(true)
int(10)
Win\System\Semaphore::getCount() expects exactly 0 parameters, 1 given
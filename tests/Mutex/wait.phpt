--TEST--
Win\System\Mutex->wait() method
--SKIPIF--
<?php
include __DIR__ . '/../skipif.inc';
?>
--FILE--
<?php
use Win\System\Mutex;
use Win\System\InvalidArgumentException;

$mutex = new Mutex();

var_dump($mutex->wait());
var_dump($mutex->wait(30));
var_dump($mutex->wait(30, true));

// no more then 2 args
try {
   var_dump($mutex->wait(1, 1, 1));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be int
try {
    var_dump($mutex->wait(array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be boolean
try {
    var_dump($mutex->wait(1, array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(0)
int(0)
int(0)
Win\System\Mutex::wait() expects at most 2 parameters, 3 given
Win\System\Mutex::wait() expects parameter 1 to be long, array given
Win\System\Mutex::wait() expects parameter 2 to be boolean, array given
--TEST--
Win\System\Mutex->waitMsg() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Mutex;
use Win\System\WaitMask;
use Win\System\ArgumentException;

$mutex = new Mutex();

var_dump($mutex->waitMsg(30));
var_dump($mutex->waitMsg(30, WaitMask::ALLINPUT));
var_dump($mutex->waitMsg(30, WaitMask::ALLINPUT, true));

// no more then 3 args
try {
   var_dump($mutex->waitMsg(1, 1, 1, 1));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be int
try {
    var_dump($mutex->waitMsg(array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be int
try {
    var_dump($mutex->waitMsg(1, array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be boolean
try {
    var_dump($mutex->waitMsg(1, 1, array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(0)
int(0)
int(0)
Win\System\Mutex::waitMsg() expects at most 3 parameters, 4 given
Win\System\Mutex::waitMsg() expects parameter 1 to be long, array given
Win\System\Mutex::waitMsg() expects parameter 2 to be long, array given
Win\System\Mutex::waitMsg() expects parameter 3 to be boolean, array given
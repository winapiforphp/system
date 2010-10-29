--TEST--
Win\System\Wait::multipleMsg() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Wait;
use Win\System\WaitMask;
use Win\System\ArgumentException;
use Win\System\Event;
use Win\System\Semaphore;
use Win\System\Mutex;

// create objects
$objects = array(new Event('event'),
                 new Semaphore('semaphore'),
                 new Mutex('mutex')
            );

var_dump(Wait::multipleMsg($objects));

var_dump(Wait::multipleMsg($objects, 30));

var_dump(Wait::multipleMsg($objects, 30, WaitMask::ALLINPUT));

var_dump(Wait::multipleMsg($objects, 30, WaitMask::ALLINPUT, WAIT::ALERTABLE));

// requires at least 1 arg
try {
    var_dump(Wait::multipleMsg());
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// requires no more then 4 args
try {
    var_dump(Wait::multipleMsg(array(), 1, 1, 1, 1));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be array or hashtableable (as in arrayobject)
try {
    var_dump(Wait::multipleMsg(1));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be integer
try {
    var_dump(Wait::multipleMsg(array(), array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be integer
try {
    var_dump(Wait::multipleMsg(array(), 1, array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 4 must be integer
try {
    var_dump(Wait::multipleMsg(array(), 1, 1, array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(0)
int(2)
int(2)
int(2)
Win\System\Wait::multipleMsg() expects at least 1 parameter, 0 given
Win\System\Wait::multipleMsg() expects at most 4 parameters, 5 given
Win\System\Wait::multipleMsg() expects parameter 1 to be array, integer given
Win\System\Wait::multipleMsg() expects parameter 2 to be long, array given
Win\System\Wait::multipleMsg() expects parameter 3 to be long, array given
Win\System\Wait::multipleMsg() expects parameter 4 to be long, array given
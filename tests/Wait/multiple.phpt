--TEST--
Win\System\Wait::multiple() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Wait;
use Win\System\ArgumentException;
use Win\System\Event;
use Win\System\Semaphore;
use Win\System\Mutex;

// create objects
$objects = array(new Event('event'),
                 new Semaphore('semaphore'),
                 new Mutex('mutex')
            );

var_dump(Wait::multiple($objects));

var_dump(Wait::multiple($objects, 30));

var_dump(Wait::multiple($objects, 30, true));

var_dump(Wait::multiple($objects, 30, true, true));

// requires at least 1 arg
try {
    var_dump(Wait::multiple());
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// requires no more then 4 args
try {
    var_dump(Wait::multiple(array(), 1, 1, 1, 1));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be array or hashtableable (as in arrayobject)
try {
    var_dump(Wait::multiple(1));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be integer
try {
    var_dump(Wait::multiple(array(), array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be bool
try {
    var_dump(Wait::multiple(array(), 1, array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 4 must be bool
try {
    var_dump(Wait::multiple(array(), 1, 1, array()));
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(0)
int(2)
int(258)
int(258)
Win\System\Wait::multiple() expects at least 1 parameter, 0 given
Win\System\Wait::multiple() expects at most 4 parameters, 5 given
Win\System\Wait::multiple() expects parameter 1 to be array, integer given
Win\System\Wait::multiple() expects parameter 2 to be long, array given
Win\System\Wait::multiple() expects parameter 3 to be boolean, array given
Win\System\Wait::multiple() expects parameter 4 to be boolean, array given
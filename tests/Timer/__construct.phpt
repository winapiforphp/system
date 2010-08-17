--TEST--
Win\System\Timer->__construct() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Timer;
use Win\System\Semaphore;

// new unnamed timer, have to pass by object now
$timer = new Timer();
var_dump($timer->getName());
var_dump($timer->isAutoReset());

// new named timer
$timer = new Timer('foobar');
var_dump($timer->getName());
var_dump($timer->isAutoReset());

// try to open semaphore with same name
try {
    $semaphore = new Semaphore('foobar');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// new named timer autoreset
$timer = new Timer('silly', true);
var_dump($timer->getName());
var_dump($timer->isAutoReset());

// new unnamed timer can inherit
$timer = new Timer(null, false, true);
var_dump($timer->getName());
var_dump($timer->isAutoReset());

// requires 0-3 args, 4 is too many
try {
    $timer = new Timer(1, 1, 1, 1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be stringable
try {
    $timer = new Timer(array(), 1, 1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be booleanable
try {
    $timer = new Timer('string', array(), 1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be booleanable
try {
    $timer = new Timer('string', 1, array());
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
NULL
bool(true)
string(6) "foobar"
bool(true)
Could not create or open the requested semaphore
string(5) "silly"
bool(true)
NULL
bool(false)
Win\System\Timer::__construct() expects at most 3 parameters, 4 given
Win\System\Timer::__construct() expects parameter 1 to be string, array given
Win\System\Timer::__construct() expects parameter 2 to be boolean, array given
Win\System\Timer::__construct() expects parameter 3 to be boolean, array given
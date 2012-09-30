--TEST--
Win\System\Mutex->signalAndWait() method
--SKIPIF--
<?php
include __DIR__ . '/../skipif.inc';
?>
--FILE--
<?php
use Win\System\Mutex;
use Win\System\InvalidArgumentException;

// create two mutexes
$mutex = new Mutex();
$signal = new Mutex('signal', true);

var_dump($mutex->signalAndWait($signal));
// reown our mutex
$signal->wait();
var_dump($mutex->signalAndWait($signal, 30));
// reown our mutex
$signal->wait();
var_dump($mutex->signalAndWait($signal, 30, true));

// requires 1 arg
try {
   var_dump($mutex->signalAndWait());
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// no more then 3 args
try {
   var_dump($mutex->signalAndWait($signal, 1, 1, 1));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be waitable
try {
    var_dump($mutex->signalAndWait(array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be int
try {
    var_dump($mutex->signalAndWait($signal, array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be boolean
try {
    var_dump($mutex->signalAndWait($signal, 1, array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(0)
int(0)
int(0)
Win\System\Mutex::signalAndWait() expects at least 1 parameter, 0 given
Win\System\Mutex::signalAndWait() expects at most 3 parameters, 4 given
Win\System\Mutex::signalAndWait() expects parameter 1 to be Win\System\Waitable, array given
Win\System\Mutex::signalAndWait() expects parameter 2 to be long, array given
Win\System\Mutex::signalAndWait() expects parameter 3 to be boolean, array given
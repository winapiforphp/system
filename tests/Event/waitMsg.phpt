--TEST--
Win\System\Event->waitMsg() method
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\WaitMask;
use Win\System\InvalidArgumentException;

$event = new Event();

var_dump($event->waitMsg(30));
var_dump($event->waitMsg(30, WaitMask::ALLINPUT));
var_dump($event->waitMsg(30, WaitMask::ALLINPUT, true));

// no more then 3 args
try {
   var_dump($event->waitMsg(1, 1, 1, 1));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be int
try {
    var_dump($event->waitMsg(array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be int
try {
    var_dump($event->waitMsg(1, array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be boolean
try {
    var_dump($event->waitMsg(1, 1, array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(0)
int(258)
int(258)
Win\System\Event::waitMsg() expects at most 3 parameters, 4 given
Win\System\Event::waitMsg() expects parameter 1 to be long, array given
Win\System\Event::waitMsg() expects parameter 2 to be long, array given
Win\System\Event::waitMsg() expects parameter 3 to be boolean, array given
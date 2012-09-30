--TEST--
Win\System\Event->signalAndWait() method
--SKIPIF--
<?php
include __DIR__ . '/../skipif.inc';
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\InvalidArgumentException;

// create two events
$event = new Event();
$signal = new Event();

var_dump($event->signalAndWait($signal));
var_dump($event->signalAndWait($signal, 30));
var_dump($event->signalAndWait($signal, 30, true));

// requires 1 arg
try {
   var_dump($event->signalAndWait());
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// no more then 3 args
try {
   var_dump($event->signalAndWait($signal, 1, 1, 1));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be waitable
try {
    var_dump($event->signalAndWait(array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be int
try {
    var_dump($event->signalAndWait($signal, array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be boolean
try {
    var_dump($event->signalAndWait($signal, 1, array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(0)
int(258)
int(258)
Win\System\Event::signalAndWait() expects at least 1 parameter, 0 given
Win\System\Event::signalAndWait() expects at most 3 parameters, 4 given
Win\System\Event::signalAndWait() expects parameter 1 to be Win\System\Waitable, array given
Win\System\Event::signalAndWait() expects parameter 2 to be long, array given
Win\System\Event::signalAndWait() expects parameter 3 to be boolean, array given
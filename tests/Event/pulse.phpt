--TEST--
Win\System\Event->pulse() method
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\InvalidArgumentException;

// create an Event
$event = new Event();

// pulse it
var_dump($event->pulse());

// pulse it again - no errors here
var_dump($event->pulse());

// bad number of args
try {
    $event->pulse(1);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
Win\System\Event::pulse() expects exactly 0 parameters, 1 given
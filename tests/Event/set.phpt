--TEST--
Win\System\Event->set() method
--SKIPIF--
<?php
include __DIR__ . '/../skipif.inc';
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\InvalidArgumentException;

// create an Event
$event = new Event();

// set it
var_dump($event->set());

// set it again - no errors here
var_dump($event->set());

// bad number of args
try {
    $event->set(1);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
Win\System\Event::set() expects exactly 0 parameters, 1 given
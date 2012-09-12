--TEST--
Win\System\Event->reset() method
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\InvalidArgumentException;

// create a non auto reset Event
$event = new Event(null, true);

// reset it
var_dump($event->reset());

// reset it again - no errors here
var_dump($event->reset());

// bad number of args
try {
    $event->reset(1);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
Win\System\Event::reset() expects exactly 0 parameters, 1 given
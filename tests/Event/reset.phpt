--TEST--
Win\System\Event->reset() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\ArgumentException;

// create a non auto reset Event
$event = new Event(null, true);

// reset it
var_dump($event->reset());

// reset it again - no errors here
var_dump($event->reset());

// bad number of args
try {
    $event->reset(1);
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
Win\System\Event::reset() expects exactly 0 parameters, 1 given
--TEST--
Win\System\Event->set() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\ArgumentException;

// create an Event
$event = new Event();

// set it
var_dump($event->set());

// set it again - no errors here
var_dump($event->set());

// bad number of args
try {
    $event->set(1);
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
Win\System\Event::set() expects exactly 0 parameters, 1 given
--TEST--
Win\System\Event->pulse() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\Exception;

// create an Event
$event = new Event();

// pulse it
var_dump($event->pulse());

// pulse it again - no errors here
var_dump($event->pulse());

// bad number of args
try {
    $event->pulse(1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
Win\System\Event::pulse() expects exactly 0 parameters, 1 given
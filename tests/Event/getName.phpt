--TEST--
Win\System\Event->getName() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\Exception;

// create an unnamed event
$event = new Event();
var_dump($event->getName());

// create a a event with a null name
$event = new Event(null);
var_dump($event->getName());

// create a event with an empty string for a name
$event = new Event('');
var_dump($event->getName());

// create a named event
$event = new Event('foobar');
var_dump($event->getName());

// bad number of args
try {
    $event->getName(1);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
NULL
NULL
string(0) ""
string(6) "foobar"
Win\System\Event::getName() expects exactly 0 parameters, 1 given
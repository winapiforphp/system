--TEST--
Win\System\Event->isAutoReset() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\ArgumentException;

// create an unnamed event
$event = new Event();

// default is autoreset on
var_dump($event->isAutoReset());

// create an an unnamed even with autoreset off
$event = new Event(null, false, false);

// now should be off
var_dump($event->isAutoReset());

// bad number of args
try {
    $event->isAutoReset(1);
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(false)
Win\System\Event::isAutoReset() expects exactly 0 parameters, 1 given
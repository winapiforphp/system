--TEST--
Win\System\Event->canInherit() method
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Event;
use  Win\System\InvalidArgumentException;

// create a normal event, default inherit is true
$event = new Event();
var_dump($event->canInherit());

// create it explicitly off
$event = new Event(null, false, false, false);
var_dump($event->canInherit());

// create it explicitly on
$event = new Event(null, false, false, true);
var_dump($event->canInherit());

// bad number of args
try {
    $event->canInherit(1);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
Win\System\Event::canInherit() expects exactly 0 parameters, 1 given
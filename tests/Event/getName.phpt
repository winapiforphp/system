--TEST--
Win\System\Event->getName() method
--SKIPIF--
<?php
include __DIR__ . '/../skipif.inc';
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\Unicode;
use Win\System\CodePage;
use Win\System\InvalidArgumentException;

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

// create a unicode semaphore
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, new CodePage(CodePage::UTF8));
$event = new Event($unicode);
var_dump($event->getName() === $unicode);

unset($unicode);
var_dump($event->getName() instanceof Unicode);

// bad number of args
try {
    $event->getName(1);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
unset($event);
?>
--EXPECT--
NULL
NULL
string(0) ""
string(6) "foobar"
bool(true)
bool(true)
Win\System\Event::getName() expects exactly 0 parameters, 1 given
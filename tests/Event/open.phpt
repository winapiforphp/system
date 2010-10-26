--TEST--
Win\System\Event::open() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Mutex;
use Win\System\Event;
use Win\System\Unicode;
use Win\System\CodePage;
use Win\System\ArgumentException;
use Win\System\Exception;

// create new named event
$event = new Event('foobar');

// open that event
$event = Event::open('foobar');

// new unicode event
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, CodePage::UTF8);
$event = new Event($unicode);

// open unicode semaphore
$event = Event::open($unicode);

// non-existent semaphore
try {
    $event = Event::open('hello');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// requires at least 1 arg
try {
    $event = Event::open();
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// requires 1-2 args, 3 is too many
try {
    $event = Event::open('foobar', 1, 1);
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be stringable or instanceof Unicode
try {
    $event = Event::open(array());
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be booleanable
try {
    $event = Event::open('foobar', array());
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Event was not found and could not be opened
Win\System\Event::open() expects at least 1 parameter, 0 given
Win\System\Event::open() expects at most 2 parameters, 3 given
Win\System\Event::open() expects parameter 1 to be string, array given
Win\System\Event::open() expects parameter 2 to be boolean, array given
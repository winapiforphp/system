--TEST--
Win\System\Event->__construct() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\Mutex;
use Win\System\Unicode;
use Win\System\CodePage;
use Win\System\ArgumentException;

// new unnamed event, have to pass by object now
$event = new Event();
var_dump($event->getName());

// new named event
$event = new Event('foobar');
var_dump($event->getName());

// try to open mutex with same name
try {
    $mutex = new Mutex('foobar');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// new unicode
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, CodePage::UTF8);
$event = new Event($unicode);
var_dump($event->getName() === $unicode);

// try to open mutex with same name
try {
    $mutex = new Mutex($unicode);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// new named event with state to tru
$event = new Event('silly', true);
var_dump($event->getName());

// named event is autoreset
$event = new Event('foo', null, true);
var_dump($event->getName());
var_dump($event->isAutoReset());

// named event can inherit
$event = new Event('bar', null, false, true);
var_dump($event->getName());
var_dump($event->canInherit());

// requires 0-4 args, 5 is too many
try {
    $event = new Event(1, 1, 1, 1, 1);
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be stringable
try {
    $event = new Event(array());
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be booleanable
try {
    $event = new Event('string', array());
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be booleanable
try {
    $event = new Event('string', 1, array());
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 4 must be booleanable
try {
    $event = new Event('string', 1, 1, array());
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
NULL
string(6) "foobar"
Name is already in use for waitable object
bool(true)
Name is already in use for waitable object
string(5) "silly"
string(3) "foo"
bool(true)
string(3) "bar"
bool(true)
Win\System\Event::__construct() expects at most 4 parameters, 5 given
Win\System\Event::__construct() expects parameter 1 to be string, array given
Win\System\Event::__construct() expects parameter 2 to be boolean, array given
Win\System\Event::__construct() expects parameter 3 to be boolean, array given
Win\System\Event::__construct() expects parameter 4 to be boolean, array given
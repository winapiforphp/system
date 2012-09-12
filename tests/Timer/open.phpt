--TEST--
Win\System\Timer::open() method
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Timer;
use Win\System\Event;
use Win\System\Unicode;
use Win\System\CodePage;
use Win\System\InvalidArgumentException;
use Win\System\RuntimeException;

// create new named timer
$timer = new Timer('foobar');

// open that timer
$timer = Timer::open('foobar');

// new unicode timer
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, new CodePage(CodePage::UTF8));
$timer = new Timer($unicode);

// open unicode timer
$timer = Timer::open($unicode);

// non-existent timer
try {
    var_dump(Timer::open('hello'));
} catch (RuntimeException $e) {
    echo $e->getMessage(), "\n";
}

// requires at least 1 arg
try {
    var_dump(Timer::open());
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// requires 1-2 args, 3 is too many
try {
    var_dump(Timer::open('foobar', 1, 1));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be stringable or instanceof Unicode
try {
    var_dump(Timer::open(array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be booleanable
try {
    var_dump(Timer::open('foobar', array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Timer was not found and could not be opened
Win\System\Timer::open() expects at least 1 parameter, 0 given
Win\System\Timer::open() expects at most 2 parameters, 3 given
Win\System\Timer::open() expects parameter 1 to be string, array given
Win\System\Timer::open() expects parameter 2 to be boolean, array given
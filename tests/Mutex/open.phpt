--TEST--
Win\System\Mutex::open() method
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

// create new named mutex
$mutex = new Mutex('foobar');

// open that mutex
$mutex = Mutex::open('foobar');

// new unicode mutex
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, CodePage::UTF8);
$mutex = new Mutex($unicode);

// open unicode mutex
$mutex = Mutex::open($unicode);

// non-existent mutex
try {
    $mutex = Mutex::open('hello');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// requires at least 1 arg
try {
    $mutex = Mutex::open();
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// requires 1-2 args, 3 is too many
try {
    $mutex = Mutex::open('foobar', 1, 1);
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be stringable or instanceof Unicode
try {
    $mutex = Mutex::open(array());
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be booleanable
try {
    $mutex = Mutex::open('foobar', array());
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Mutex was not found and could not be opened
Win\System\Mutex::open() expects at least 1 parameter, 0 given
Win\System\Mutex::open() expects at most 2 parameters, 3 given
Win\System\Mutex::open() expects parameter 1 to be string, array given
Win\System\Mutex::open() expects parameter 2 to be boolean, array given
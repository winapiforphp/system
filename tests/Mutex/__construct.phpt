--TEST--
Win\System\Mutex->__construct() method
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

// new unnamed mutex, have to pass by object now
$mutex = new Mutex();
var_dump($mutex->getName());

// new named mutex
$mutex = new Mutex('foobar');
var_dump($mutex->getName());

// try to open and own same mutex - will blow up
try {
    $mutex = new Mutex('foobar', true);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// try to open event with same name
try {
    $event = new Event('foobar');
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// new unicode mutex
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, CodePage::UTF8);
$mutex = new Mutex($unicode);
var_dump($mutex->getName() === $unicode);

// try to open and own same mutex - will blow up
try {
    $mutex = new Mutex($unicode, true);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// try to open event with same name
try {
    $event = new Event($unicode);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// new named mutex created then open
$mutex1 = new Mutex('my mutex');
// different mutex object - same underlying C mutex (opened not created)
$mutex2 = new Mutex('my mutex');

// new named mutex owned
$mutex = new Mutex('silly', true);
var_dump($mutex->getName());

// new unnamed mutex can inherit
$mutex = new Mutex(null, false, true);
var_dump($mutex->getName());

// requires 0-3 args, 4 is too many
try {
    $mutex = new Mutex(1, 1, 1, 1);
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 1 must be stringable or instanceof Unicode
try {
    $mutex = new Mutex(array(), 1, 1);
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be booleanable
try {
    $mutex = new Mutex('string', array(), 1);
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be booleanable
try {
    $mutex = new Mutex('string', 1, array());
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
NULL
string(6) "foobar"
Mutex could not be created and marked as owned
Name is already in use for waitable object
bool(true)
Mutex could not be created and marked as owned
Name is already in use for waitable object
string(5) "silly"
NULL
Win\System\Mutex::__construct() expects at most 3 parameters, 4 given
Win\System\Mutex::__construct() expects parameter 1 to be string, array given
Win\System\Mutex::__construct() expects parameter 2 to be boolean, array given
Win\System\Mutex::__construct() expects parameter 3 to be boolean, array given

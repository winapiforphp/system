--TEST--
Win\System\Mutex->getName() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Mutex;
use Win\System\ArgumentException;
use Win\System\Unicode;
use Win\System\CodePage;

// create an unnamed mutex
$mutex = new Mutex();
var_dump($mutex->getName());

// create a a mutex with a null name
$mutex = new Mutex(null);
var_dump($mutex->getName());

// create a mutex with an empty string for a name
$mutex = new Mutex('');
var_dump($mutex->getName());

// create a named mutex
$mutex = new Mutex('foobar');
var_dump($mutex->getName());

// create a unicode mutex
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, CodePage::UTF8);
$mutex = new Mutex($unicode);
var_dump($mutex->getName() === $unicode);

unset($unicode);
var_dump($mutex->getName() instanceof Unicode);

// bad number of args
try {
    $mutex->getName(1);
} catch (ArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
NULL
NULL
string(0) ""
string(6) "foobar"
bool(true)
bool(true)
Win\System\Mutex::getName() expects exactly 0 parameters, 1 given
--TEST--
Win\System\Timer->getName() method
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Timer;
use Win\System\InvalidArgumentException;
use Win\System\Unicode;
use Win\System\CodePage;

// create an unnamed timer
$timer = new Timer();
var_dump($timer->getName());

// create a a timer with a null name
$timer = new Timer(null);
var_dump($timer->getName());

// create a timer with an empty string for a name
$timer = new Timer('');
var_dump($timer->getName());

// create a named timer
$timer = new Timer('foobar');
var_dump($timer->getName());

// create a unicode mutex
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, new CodePage(CodePage::UTF8));
$timer = new Timer($unicode);
var_dump($timer->getName() === $unicode);

unset($unicode);
var_dump($timer->getName() instanceof Unicode);

// bad number of args
try {
    $timer->getName(1);
} catch (InvalidArgumentException $e) {
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
Win\System\Timer::getName() expects exactly 0 parameters, 1 given
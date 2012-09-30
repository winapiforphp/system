--TEST--
Win\System\Semaphore->getName() method
--SKIPIF--
<?php
include __DIR__ . '/../skipif.inc';
?>
--FILE--
<?php
use Win\System\Semaphore;
use Win\System\InvalidArgumentException;
use Win\System\Unicode;
use Win\System\CodePage;

// create an unnamed semaphore
$semaphore = new Semaphore();
var_dump($semaphore->getName());

// create a a semaphore with a null name
$semaphore = new Semaphore(null);
var_dump($semaphore->getName());

// create a semaphore with an empty string for a name
$semaphore = new Semaphore('');
var_dump($semaphore->getName());

// create a named semaphore
$semaphore = new Semaphore('foobar');
var_dump($semaphore->getName());

// create a unicode semaphore
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, new CodePage(CodePage::UTF8));
$semaphore = new Semaphore($unicode);
var_dump($semaphore->getName() === $unicode);

unset($unicode);
var_dump($semaphore->getName() instanceof Unicode);

// bad number of args
try {
    $semaphore->getName(1);
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
Win\System\Semaphore::getName() expects exactly 0 parameters, 1 given
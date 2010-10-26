--TEST--
Win\System\Semaphore clone
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Semaphore;
use Win\System\Unicode;
use Win\System\CodePage;

$sem1 = new Semaphore('clone');
var_dump($sem1->getName());

$sem2 = clone $sem1;
var_dump($sem2->getName());

var_dump($sem1 !== $sem2);

// new unicode mutex
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, CodePage::UTF8);
$sem1 = new Semaphore($unicode);

$sem2 = clone $sem1;
var_dump($sem1 !== $sem2);
var_dump($sem1->getName() === $sem2->getName());
?>
--EXPECT--
string(5) "clone"
string(5) "clone"
bool(true)
bool(true)
bool(true)
--TEST--
Win\System\Mutex clone
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Mutex;
use Win\System\Unicode;
use Win\System\CodePage;

$mutex1 = new Mutex('clone');
var_dump($mutex1->getName());

$mutex2 = clone $mutex1;
var_dump($mutex2->getName());

var_dump($mutex1 !== $mutex2);

// new unicode mutex
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, new CodePage(CodePage::UTF8));
$mutex1 = new Mutex($unicode);

$mutex2 = clone $mutex1;
var_dump($mutex1 !== $mutex2);
var_dump($mutex1->getName() === $mutex2->getName());
?>
--EXPECT--
string(5) "clone"
string(5) "clone"
bool(true)
bool(true)
bool(true)
--TEST--
Win\System\Event clone
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\Unicode;
use Win\System\CodePage;

$event1 = new Event('clone');
var_dump($event1->getName());

$event2 = clone $event1;
var_dump($event2->getName());
var_dump($event1 !== $event2);

// new unicode mutex
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, CodePage::UTF8);
$event1 = new Event($unicode);

$event2 = clone $event1;
var_dump($event1 !== $event2);
var_dump($event1->getName() === $event2->getName());
?>
--EXPECT--
string(5) "clone"
string(5) "clone"
bool(true)
bool(true)
bool(true)
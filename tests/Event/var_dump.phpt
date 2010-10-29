--TEST--
Win\System\Event var_dump
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\Unicode;
use Win\System\CodePage;

$event = new Event('clone');
var_dump($event);

$event = new Event('foobar', true);
var_dump($event);

// new unicode mutex
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, CodePage::UTF8);
$event = new Event($unicode);

var_dump($event);
?>
--EXPECTF--
object(Win\System\Event)#%d (2) {
  ["canInherit":"Win\System\Event":private]=>
  bool(true)
  ["name":"Win\System\Event":private]=>
  string(5) "clone"
}
object(Win\System\Event)#%d (2) {
  ["canInherit":"Win\System\Event":private]=>
  bool(true)
  ["name":"Win\System\Event":private]=>
  string(6) "foobar"
}
object(Win\System\Event)#%d (2) {
  ["canInherit":"Win\System\Event":private]=>
  bool(true)
  ["name":"Win\System\Event":private]=>
  object(Win\System\Unicode)#%d (0) {
  }
}
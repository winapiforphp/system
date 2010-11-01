--TEST--
Win\System\Timer var_dump
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Timer;
use Win\System\Unicode;
use Win\System\CodePage;

$timer = new Timer('clone');
var_dump($timer);

$timer = new Timer('foobar', false, true);
var_dump($timer);

// new unicode mutex
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, CodePage::UTF8);
$timer = new Timer($unicode);

var_dump($timer);
?>
--EXPECTF--
object(Win\System\Timer)#%d (2) {
  ["canInherit":"Win\System\Timer":private]=>
  bool(true)
  ["name":"Win\System\Timer":private]=>
  string(5) "clone"
}
object(Win\System\Timer)#%d (2) {
  ["canInherit":"Win\System\Timer":private]=>
  bool(true)
  ["name":"Win\System\Timer":private]=>
  string(6) "foobar"
}
object(Win\System\Timer)#%d (2) {
  ["canInherit":"Win\System\Timer":private]=>
  bool(true)
  ["name":"Win\System\Timer":private]=>
  object(Win\System\Unicode)#%d (0) {
  }
}
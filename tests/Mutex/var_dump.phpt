--TEST--
Win\System\Mutex var_dump
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Mutex;
use Win\System\Unicode;
use Win\System\CodePage;

$mutex = new Mutex('clone');
var_dump($mutex);

$mutex = new Mutex('foobar', true);
var_dump($mutex);

// new unicode mutex
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, CodePage::UTF8);
$mutex = new Mutex($unicode);

var_dump($mutex);
?>
--EXPECTF--
object(Win\System\Mutex)#%d (3) {
  ["isOwned":"Win\System\Mutex":private]=>
  bool(false)
  ["canInherit":"Win\System\Mutex":private]=>
  bool(true)
  ["name":"Win\System\Mutex":private]=>
  string(5) "clone"
}
object(Win\System\Mutex)#%d (3) {
  ["isOwned":"Win\System\Mutex":private]=>
  bool(true)
  ["canInherit":"Win\System\Mutex":private]=>
  bool(true)
  ["name":"Win\System\Mutex":private]=>
  string(6) "foobar"
}
object(Win\System\Mutex)#%d (3) {
  ["isOwned":"Win\System\Mutex":private]=>
  bool(false)
  ["canInherit":"Win\System\Mutex":private]=>
  bool(true)
  ["name":"Win\System\Mutex":private]=>
  object(Win\System\Unicode)#%d (0) {
  }
}
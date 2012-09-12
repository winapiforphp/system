--TEST--
Win\System\Mutex var_dump
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
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
$unicode = new Unicode($string, new CodePage(CodePage::UTF8));
$mutex = new Mutex($unicode);

var_dump($mutex);
?>
--EXPECTF--
object(Win\System\Mutex)#%d (2) {
  ["canInherit":"Win\System\Mutex":private]=>
  bool(true)
  ["name":"Win\System\Mutex":private]=>
  string(5) "clone"
}
object(Win\System\Mutex)#%d (2) {
  ["canInherit":"Win\System\Mutex":private]=>
  bool(true)
  ["name":"Win\System\Mutex":private]=>
  string(6) "foobar"
}
object(Win\System\Mutex)#%d (2) {
  ["canInherit":"Win\System\Mutex":private]=>
  bool(true)
  ["name":"Win\System\Mutex":private]=>
  object(Win\System\Unicode)#%d (0) {
  }
}
--TEST--
Win\System\Semaphore var_dump
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Semaphore;
use Win\System\Unicode;
use Win\System\CodePage;

$semaphore = new Semaphore('clone');
var_dump($semaphore);

$semaphore = new Semaphore('foobar', true);
var_dump($semaphore);

// new unicode mutex
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, new CodePage(CodePage::UTF8));
$semaphore = new Semaphore($unicode);

var_dump($semaphore);
?>
--EXPECTF--
object(Win\System\Semaphore)#%d (3) {
  ["maxCount":"Win\System\Semaphore":private]=>
  int(1)
  ["canInherit":"Win\System\Semaphore":private]=>
  bool(true)
  ["name":"Win\System\Semaphore":private]=>
  string(5) "clone"
}
object(Win\System\Semaphore)#%d (3) {
  ["maxCount":"Win\System\Semaphore":private]=>
  int(1)
  ["canInherit":"Win\System\Semaphore":private]=>
  bool(true)
  ["name":"Win\System\Semaphore":private]=>
  string(6) "foobar"
}
object(Win\System\Semaphore)#%d (3) {
  ["maxCount":"Win\System\Semaphore":private]=>
  int(1)
  ["canInherit":"Win\System\Semaphore":private]=>
  bool(true)
  ["name":"Win\System\Semaphore":private]=>
  object(Win\System\Unicode)#%d (0) {
  }
}
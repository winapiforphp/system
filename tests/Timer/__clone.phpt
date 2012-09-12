--TEST--
Win\System\Timer clone
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Timer;
use Win\System\Wait;
use Win\System\Unicode;
use Win\System\CodePage;

class dotimer {
    public function mytimer() {
      echo "timer\n";
    }
}

$timer1 = new Timer('clone');
var_dump($timer1->getName());

$timer2 = clone $timer1;
var_dump($timer2->getName());
var_dump($timer1 !== $timer2);

// new unicode mutex
$string = 'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥';
$unicode = new Unicode($string, new CodePage(CodePage::UTF8));
$timer1 = new Timer($unicode);

$timer2 = clone $timer1;
var_dump($timer1 !== $timer2);
var_dump($timer1->getName() === $timer2->getName());

/* Trickier - timer with callback set */
$obj = new dotimer();
$timer1 = new Timer('clone', false);
var_dump($timer1->set(10, 10, false, array($obj, 'mytimer')));

$timer2 = clone $timer1;
var_dump($timer1 !== $timer2);

var_dump($timer1->wait(30, true));
var_dump($timer1->wait(30, true));
unset($timer1);

var_dump($timer2->wait(30, true));
var_dump($timer2->wait(30, true));
?>
--EXPECT--
string(5) "clone"
string(5) "clone"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(0)
timer
int(192)
int(0)
timer
int(192)
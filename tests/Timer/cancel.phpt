--TEST--
Win\System\Timer->cancel() method
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Timer;
use Win\System\InvalidArgumentException;

function mytimer () {}

class dotimer {
    public function mytimer() {}
}

// create a timer
$timer = new Timer();

// cancel without a set
var_dump($timer->cancel());

// set with no callback and cancel
var_dump($timer->set(10, 0));
var_dump($timer->cancel());

// set with function callback, no args, and cancel
var_dump($timer->set(10, 0, false, 'mytimer'));
var_dump($timer->cancel());

// set with function callback with args
var_dump($timer->set(10, 0, false, 'mytimer', 'foo', 'bar'));
var_dump($timer->cancel());

// set with object pointer
$obj = new dotimer();
var_dump($timer->set(10, 0, false, array($obj, 'mytimer')));
var_dump($timer->cancel());

// bad number of args
try {
    $timer->cancel(1);
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Win\System\Timer::cancel() expects exactly 0 parameters, 1 given
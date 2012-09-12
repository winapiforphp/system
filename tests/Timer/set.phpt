--TEST--
Win\System\Timer->set() method
--SKIPIF--
<?php
include __DIR__ . '/../../skipif.inc';
?>
--FILE--
<?php
use Win\System\Timer;
use Win\System\InvalidArgumentException;

function timer () {
    echo "Timing\n";
}

function mytimer ($foo, $bar) {
    echo "I am timing! $foo, $bar\n";
}

class dotimer {
    public function mytimer() {
      echo "timer\n";
    }
}

$obj = new dotimer();

// regular timer
$timer = new Timer('mytimer');
var_dump($timer->set(10));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));

// regular timer with callback
var_dump($timer->set(10, 0, false, 'timer'));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));

// regular timer with object
var_dump($timer->set(10, 0, false, array($obj, 'mytimer')));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));

// regular timer with args
var_dump($timer->set(10, 10, false, 'mytimer', 'foo', 'bar'));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));

// periodic timer
$timer = new Timer('mytimer', false);
var_dump($timer->set(10, 10));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));

// periodic timer with callback
var_dump($timer->set(10, 10, false, 'timer'));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));

// periodic timer with object
var_dump($timer->set(10, 10, false, array($obj, 'mytimer')));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));

// periodic timer with args
var_dump($timer->set(10, 10, false, 'mytimer', 'foo', 'bar'));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));
var_dump($timer->wait(30, true));

// Requires at least 1 arg
try {
    var_dump($timer->set());
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// never too many args
var_dump($timer->set(1, 1, 1, 'mytimer', 1, 1, 1, 1));

// arg 1 must be int
try {
    var_dump($timer->set(array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 2 must be int
try {
    var_dump($timer->set(1, array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 3 must be bool
try {
    var_dump($timer->set(1, 1, array()));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}

// arg 4 must be callable
try {
    var_dump($timer->set(1, 1, 1, ''));
} catch (InvalidArgumentException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
int(0)
int(0)
bool(true)
int(0)
Timing
int(192)
int(0)
bool(true)
int(0)
timer
int(192)
int(0)
bool(true)
int(0)
I am timing! foo, bar
int(192)
int(0)
bool(true)
int(0)
int(0)
bool(true)
int(0)
Timing
int(192)
int(0)
bool(true)
int(0)
timer
int(192)
int(0)
bool(true)
int(0)
I am timing! foo, bar
int(192)
int(0)
Win\System\Timer::set() expects at least 1 parameter, 0 given
bool(true)
Win\System\Timer::set() expects parameter 1 to be long, array given
Win\System\Timer::set() expects parameter 2 to be long, array given
Win\System\Timer::set() expects parameter 3 to be boolean, array given
Win\System\Timer::set() expects parameter 4 to be a valid callback, function '' not found or invalid function name
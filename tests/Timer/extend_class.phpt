--TEST--
Extending Timer
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Timer;
use Win\System\Exception;

class goodTimer extends Timer {}

class badTimer extends Timer {
    public function __construct() {
        echo "bad";
    }
}

class argTimer extends Timer {
    public function __construct($name) {
        parent::__construct($name);
    }
}

// good timer is fine
$timer = new goodTimer();
var_dump($timer->getName());

// bad timer will throw exception
try {
    $timer = new badTimer();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg timer will create a new named timer
$timer = new argTimer('foobar');
var_dump($timer->getName());
?>
--EXPECTF--
NULL
badparent::__construct() must be called in badTimer::__construct()
string(6) "foobar"
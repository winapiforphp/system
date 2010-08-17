--TEST--
Extending Timer
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Timer;

class goodTimer extends Timer {}

class badTimer extends Timer {
    public function __construct() {
        echo "bad";
    }
}

// good timer is fine
$timer = new goodTimer();
var_dump($timer->getName());

// bad timer will fatal error
$timer = new badTimer();
var_dump($timer->getName());
?>
--EXPECTF--
NULL
bad
Fatal error: Internal timer handle missing in badTimer class, you must call parent::__construct in extended classes in %s on line %d
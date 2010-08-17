--TEST--
Extending Event
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Event;
use Win\System\Exception;

class goodEvent extends Event {}

class badEvent extends Event {
    public function __construct() {
        echo "bad";
    }
}

// good event is fine
$event = new goodEvent();
var_dump($event->getName());

// bad event will fatal error
$event = new badEvent();
var_dump($event->getName());
?>
--EXPECTF--
NULL
bad
Fatal error: Internal event handle missing in badEvent class, you must call parent::__construct in extended classes in %s on line %d
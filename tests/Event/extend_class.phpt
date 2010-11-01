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

class argEvent extends Event {
    public function __construct($name) {
        parent::__construct($name);
    }
}

// good event is fine
$event = new goodEvent();
var_dump($event->getName());

// bad event will throw exception
try {
    $event = new badEvent();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg event will create a new named event
$event = new argEvent('foobar');
var_dump($event->getName());
?>
--EXPECT--
NULL
badparent::__construct() must be called in badEvent::__construct()
string(6) "foobar"
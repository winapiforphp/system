--TEST--
Extending Mutex
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Mutex;
use Win\System\Exception;

class goodMutex extends Mutex {}

class badMutex extends Mutex {
    public function __construct() {
        echo "bad";
    }
}

// good mutex is fine
$mutex = new goodMutex();
var_dump($mutex->getName());

// bad mutex will fatal error
$mutex = new badMutex();
var_dump($mutex->getName());
?>
--EXPECTF--
NULL
bad
Fatal error: Internal mutex handle missing in badMutex class, you must call parent::__construct in extended classes in %s on line %d
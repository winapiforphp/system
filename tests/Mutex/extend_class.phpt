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
        $this->release();
    }
}

// good mutex is fine
$mutex = new goodMutex();
var_dump($mutex->getName());

// bad mutex will throw exception
try {
    $mutex = new badMutex();
} catch (Exception $e) {
    echo $e->getMessage();
}
?>
--EXPECTF--
NULL
parent::__construct() must be called in badMutex::__construct()
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

class argMutex extends Mutex {
    public function __construct($name) {
        parent::__construct($name);
    }
}

// good mutex is fine
$mutex = new goodMutex();
var_dump($mutex->getName());

// bad mutex will throw exception
try {
    $mutex = new badMutex();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

// arg mutex will create a new named mutex
$mutex = new argMutex('foobar');
var_dump($mutex->getName());

?>
--EXPECTF--
NULL
parent::__construct() must be called in badMutex::__construct()
string(6) "foobar"
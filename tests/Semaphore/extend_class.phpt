--TEST--
Extending Semaphore
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
use Win\System\Semaphore;
use Win\System\Exception;

class goodSem extends Semaphore {}

class badSem extends Semaphore {
    public function __construct() {
        echo "bad";
    }
}

// good semaphore is fine
$semaphore = new goodSem();
var_dump($semaphore->getName());

// bad semaphore will fatal error
$semaphore = new badSem();
var_dump($semaphore->getName());
?>
--EXPECTF--
NULL
bad
Fatal error: Internal semaphore handle missing in badSem class, you must call parent::__construct in extended classes in %s on line %d
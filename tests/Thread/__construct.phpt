--TEST--
Win\System\Thread->__construct() method
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
if(!class_exists('Win\System\Thread')) die('skip - Threads only available in thread safe PHP');
?>
--FILE--
<?php
use Win\System\Thread;

class TestThread extends Thread {
    public function run() {
        var_dump($this);
    }
}

// new thread
$thread = new TestThread();
$thread->value = 'foobar';
var_dump($thread);
?>
--EXPECTF--

<?php
/* simple example of how to run multiple threads */
if(!extension_loaded('winsystem')) {
    die('Need php_winsystem.dll loaded in your php.ini');
}
if(!class_exists('Win\System\Thread')) {
    die('Threading is not available in your build, there are no threads in NTS builds');
}

use Win\System\Thread;

// a simple callback function that spawns subthreads
function test() {
    var_dump(func_get_args());
    echo "Run the test function\n";
    Thread::start('subthread');
    Thread::start('subthread', 'foo');
}

function subthread() {
    var_dump(func_get_args());
    echo "Create a subthread - note that putting this in test could cause recursion\n";
}

class testing {
    public $foo = 'foo';

    function test() {
        var_dump(func_get_args());

        echo "A regular method call\n";
        $this->foo = 'foo';
        var_dump($this);
    }

    static function doit() {
        var_dump(func_get_args());
        echo "A static method call\n";
    }
}

$object = new testing();
$object->foo = 'bar';

// NOTE: only userland objects currently clone correctly!
$spl = new ArrayObject(array('foo', 'baz'));
var_dump($object);

Thread::start('test', 1, 'foo', array('foo', 'bar'), 1.23432, $object);
Thread::start('subthread', 35, 'foobar');
Thread::start(array($object, 'test'));
Thread::start(array('testing', 'doit'));
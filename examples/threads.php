<?php
/* simple example of how to run multiple threads */
if(!extension_loaded('winsystem')) {
    die('Need php_winsystem.dll loaded in your php.ini');
}
if(!class_exists('Win\System\Thread')) {
    die('Threading is not available in your build, there are no threads in NTS builds');
}

use Win\System\Thread;
use Win\System\Mutex;
use Win\System\Event;

class TestSubThread extends Thread {
    function run() {
        echo "subthread is doing work\n";
    }
}

/* To create a thread, extend the thread class and
  implement the run method */
class TestThread extends Thread {
    function run() {
        // create our output mutex, we don't want to own it yet
        $mutex = new Mutex('Output');
        $i = 0;
        while($i < 10) {
            echo "$i\n";
            $i++;
        }
        $this->foo = 'foobar';
        $this->foobar();
        // To avoid ugly output, we're going to wait on our mutex
        $mutex->wait();
        var_dump($this);
        // after we've output correctly, we'll release our mutex
        $mutex->release();

        // for count manipulation sync we're going to use an event
        $event = new Event('Counting');
        $event->wait();

        // get our global variable
        $count = Thread::get('count');
        echo "Our global count was $count \n";
        $count += 1;
        // increment and set it
        Thread::set('count', $count);
        // echo it
        echo "Our global count is $count \n";
        $event->set();

        //$thread = new TestSubThread();
        //$thread->run();
    }

    function foobar() {
        echo "{$this->foo}\n";
    }
}

/* Set a global variable for all threads, notice we do this BEFORE spawning threads */
Thread::set('count', 0);

/* NOTE: this will actually create TWO objects, the one
  in the current thread, and the thread object that can
  be accessed in $this in the run method */
$thread = new TestThread();
$thread->start();
$thread->start();
$thread->start();
$thread->start();
echo "Done\n";
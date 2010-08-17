--TEST--
Win\System\Waitable Interface Reflection
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
ReflectionClass::export('Win\System\Waitable');
?>
--EXPECT--
Interface [ <internal:winsystem> interface Win\System\Waitable ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [1] {
    Method [ <internal:winsystem> abstract public method wait ] {
    }
  }
}
--TEST--
Win\System\Mutex Reflection
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
ReflectionClass::export('Win\System\Mutex');
?>
--EXPECT--
Class [ <internal:winsystem> class Win\System\Mutex implements Win\System\Waitable ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [6] {
    Method [ <internal:winsystem, ctor> public method __construct ] {

      - Parameters [3] {
        Parameter #0 [ <optional> $name ]
        Parameter #1 [ <optional> $own ]
        Parameter #2 [ <optional> $process_inherit ]
      }
    }

    Method [ <internal:winsystem> public method release ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:winsystem> public method isOwned ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:winsystem> public method getName ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:winsystem> public method canInherit ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:winsystem, prototype Win\System\Waitable> public method wait ] {

      - Parameters [0] {
      }
    }
  }
}
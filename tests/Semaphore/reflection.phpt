--TEST--
Win\System\Sempahore Reflection
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
ReflectionClass::export('Win\System\Semaphore');
?>
--EXPECT--
Class [ <internal:winsystem> class Win\System\Semaphore implements Win\System\Waitable ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [1] {
    Method [ <internal:winsystem> static public method open ] {

      - Parameters [2] {
        Parameter #0 [ <required> $name ]
        Parameter #1 [ <optional> $process_inherit ]
      }
    }
  }

  - Properties [0] {
  }

  - Methods [6] {
    Method [ <internal:winsystem, ctor> public method __construct ] {

      - Parameters [4] {
        Parameter #0 [ <optional> $name ]
        Parameter #1 [ <optional> $start ]
        Parameter #2 [ <optional> $maxcount ]
        Parameter #3 [ <optional> $inherit ]
      }
    }

    Method [ <internal:winsystem> public method release ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:winsystem> public method getMaxCount ] {

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

    Method [ <internal:winsystem, inherits Win\System\Waitable> public method wait ] {

      - Parameters [0] {
      }
    }
  }
}
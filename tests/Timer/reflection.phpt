--TEST--
Win\System\Timer Reflection
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
ReflectionClass::export('Win\System\Timer');
?>
--EXPECT--
Class [ <internal:winsystem> class Win\System\Timer implements Win\System\Waitable ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [7] {
    Method [ <internal:winsystem, ctor> public method __construct ] {

      - Parameters [3] {
        Parameter #0 [ <optional> $name ]
        Parameter #1 [ <optional> $auto_reset ]
        Parameter #2 [ <optional> $process_inherit ]
      }
    }

    Method [ <internal:winsystem> public method cancel ] {

      - Parameters [3] {
        Parameter #0 [ <required> $due ]
        Parameter #1 [ <required> $period ]
        Parameter #2 [ <required> $callback ]
      }
    }

    Method [ <internal:winsystem> public method set ] {

      - Parameters [3] {
        Parameter #0 [ <required> $due ]
        Parameter #1 [ <required> $period ]
        Parameter #2 [ <required> $callback ]
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

    Method [ <internal:winsystem> public method isAutoReset ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:winsystem, prototype Win\System\Waitable> public method wait ] {

      - Parameters [0] {
      }
    }
  }
}

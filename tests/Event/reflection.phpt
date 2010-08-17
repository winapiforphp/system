--TEST--
Win\System\Event Reflection
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
?>
--FILE--
<?php
ReflectionClass::export('Win\System\Event');
?>
--EXPECT--
Class [ <internal:winsystem> class Win\System\Event implements Win\System\Waitable ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [8] {
    Method [ <internal:winsystem, ctor> public method __construct ] {

      - Parameters [4] {
        Parameter #0 [ <optional> $name ]
        Parameter #1 [ <optional> $initial_state ]
        Parameter #2 [ <optional> $auto_reset ]
        Parameter #3 [ <optional> $process_inherit ]
      }
    }

    Method [ <internal:winsystem> public method set ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:winsystem> public method reset ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:winsystem> public method pulse ] {

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
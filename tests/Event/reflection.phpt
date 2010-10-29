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

  - Methods [9] {
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

    Method [ <internal:winsystem, prototype Win\System\Waitable> public method wait ] {

      - Parameters [2] {
        Parameter #0 [ <optional> $milliseconds ]
        Parameter #1 [ <optional> $alertable ]
      }
    }

    Method [ <internal:winsystem, prototype Win\System\Waitable> public method waitMsg ] {

      - Parameters [3] {
        Parameter #0 [ <optional> $milliseconds ]
        Parameter #1 [ <optional> $mask ]
        Parameter #2 [ <optional> $alertable ]
      }
    }

    Method [ <internal:winsystem, prototype Win\System\Waitable> public method signalAndWait ] {

      - Parameters [3] {
        Parameter #0 [ <required> $signal ]
        Parameter #1 [ <optional> $milliseconds ]
        Parameter #2 [ <optional> $alertable ]
      }
    }
  }
}
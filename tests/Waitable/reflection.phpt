--TEST--
Win\System\Waitable Reflection
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

  - Methods [3] {
    Method [ <internal:winsystem> abstract public method wait ] {

      - Parameters [2] {
        Parameter #0 [ <optional> $milliseconds ]
        Parameter #1 [ <optional> $alertable ]
      }
    }

    Method [ <internal:winsystem> abstract public method waitMsg ] {

      - Parameters [3] {
        Parameter #0 [ <optional> $milliseconds ]
        Parameter #1 [ <optional> $mask ]
        Parameter #2 [ <optional> $alertable ]
      }
    }

    Method [ <internal:winsystem> abstract public method signalAndWait ] {

      - Parameters [3] {
        Parameter #0 [ <required> $signal ]
        Parameter #1 [ <optional> $milliseconds ]
        Parameter #2 [ <optional> $alertable ]
      }
    }
  }
}
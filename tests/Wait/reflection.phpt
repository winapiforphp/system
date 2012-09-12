--TEST--
Win\System\Wait Reflection
--SKIPIF--
<?php
include __DIR__ . '/../skipif.inc';
?>
--FILE--
<?php
ReflectionClass::export('Win\System\Wait');
?>
--EXPECT--
Class [ <internal:winsystem> abstract final class Win\System\Wait ] {

  - Constants [9] {
    Constant [ integer INFINITE ] { -1 }
    Constant [ integer ABANDONED ] { 128 }
    Constant [ integer IO_COMPLETION ] { 192 }
    Constant [ integer OBJECTS ] { 0 }
    Constant [ integer TIMEOUT ] { 258 }
    Constant [ integer FAILED ] { -1 }
    Constant [ integer ALERTABLE ] { 2 }
    Constant [ integer INPUTAVAILABLE ] { 4 }
    Constant [ integer WAITALL ] { 1 }
  }

  - Static properties [0] {
  }

  - Static methods [2] {
    Method [ <internal:winsystem> static public method multiple ] {

      - Parameters [4] {
        Parameter #0 [ <required> $objects ]
        Parameter #1 [ <optional> $milliseconds ]
        Parameter #2 [ <optional> $wait_all ]
        Parameter #3 [ <optional> $alertable ]
      }
    }

    Method [ <internal:winsystem> static public method multipleMsg ] {

      - Parameters [4] {
        Parameter #0 [ <required> $objects ]
        Parameter #1 [ <optional> $milliseconds ]
        Parameter #2 [ <optional> $mask ]
        Parameter #3 [ <optional> $flags ]
      }
    }
  }

  - Properties [0] {
  }

  - Methods [0] {
  }
}
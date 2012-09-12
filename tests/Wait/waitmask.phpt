--TEST--
Win\System\WaitMask Reflection
--SKIPIF--
<?php
include __DIR__ . '/../skipif.inc';
?>
--FILE--
<?php
ReflectionClass::export('Win\System\WaitMask');
?>
--EXPECT--
Class [ <internal:winsystem> abstract final class Win\System\WaitMask ] {

  - Constants [14] {
    Constant [ integer ALLEVENTS ] { 1215 }
    Constant [ integer ALLINPUT ] { 1279 }
    Constant [ integer ALLPOSTMESSAGE ] { 256 }
    Constant [ integer HOTKEY ] { 128 }
    Constant [ integer INPUT ] { 1031 }
    Constant [ integer KEY ] { 1 }
    Constant [ integer MOUSE ] { 6 }
    Constant [ integer MOUSEBUTTON ] { 4 }
    Constant [ integer MOUSEMOVE ] { 2 }
    Constant [ integer PAINT ] { 32 }
    Constant [ integer POSTMESSAGE ] { 8 }
    Constant [ integer RAWINPUT ] { 1024 }
    Constant [ integer SENDMESSAGE ] { 64 }
    Constant [ integer TIMER ] { 16 }
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [0] {
  }
}
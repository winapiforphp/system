--TEST--
Win\System\Enum is abstract
--SKIPIF--
<?php
include __DIR__ . '/../skipif.inc';
?>
--FILE--
<?php
use Win\System\Enum as Enum;

$item = new Enum;

?>
--EXPECTF--
Fatal error: Cannot instantiate abstract class Win\System\Enum in %s on line %d

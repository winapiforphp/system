--TEST--
phpinfo specific to winsystem
--SKIPIF--
<?php
include __DIR__ . '/skipif.inc';
?>
--FILE--
<?php
ob_start();
phpinfo(INFO_MODULES);
$data = ob_get_clean();
$data = explode("\n\n", $data);
foreach($data as $key => $info) {
    if ($info === 'winsystem') {
        break;
    }
}
$data = $data[$key + 1];
var_dump($data);
?>
--EXPECTF--
string(%d) "Version => %s"
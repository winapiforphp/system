--TEST--
phpinfo specific to winsystem
--SKIPIF--
<?php
if(!extension_loaded('winsystem')) die('skip - winsystem extension not available');
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
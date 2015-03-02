<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('php_server')) {
	dl('php_server.' . PHP_SHLIB_SUFFIX);
}
$module = 'php_server';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:$br\n";
foreach($functions as $func) {
    echo $func.":".$func()."$br\n";
}
echo "$br\n";
?>

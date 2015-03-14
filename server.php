<?php

$server = new php_server('127.0.0.1',9000);

echo "server is created\n";

$server->bind('accept',function($fd,$ip,$port){
		echo "accept fd:{$fd} ip:{$ip} port:{$port}\n";
});

$server->run();

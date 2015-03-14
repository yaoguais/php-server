<?php

global $server;

$server = new php_server('127.0.0.1',9009);

print_r($server->get());

$server->bind('accept',function($fd,$ip,$port){
		echo "accept\n";
});

$server->bind('receive',function($fd,$message,$ip,$port){
		global $server;
		$server->send($fd,'nonono');
		echo "receive\n";
});

$server->bind('close',function($fd,$ip,$port){
		echo "close\n";
});

$server->run();

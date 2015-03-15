<?php

$server = new php_server('127.0.0.1',9009);

print_r($server->get());

function bind_accept($fd,$ip,$port){
	echo "accept $fd $ip $port ]\n";
}

$server->bind('accept','bind_accept');

function bind_receive($fd,$message,$ip,$port){
	//$message = rtrim($message,"\r\n");
	//echo "receive $fd $message $ip $port ]\n";
	echo $message;
	php_server_send($fd,"HTTP1.0 200 OK\r\nServer: php_server 1.0\r\nConnection: Keep-Alive\r\nContent-Length: 11\r\n\r\n0123456789");
}

$server->bind('receive','bind_receive');

function bind_close($fd,$ip,$port){
	echo "close $fd $ip $port ]\n";
}

$server->bind('close','bind_close');

/*$server->bind('accept',function($fd,$ip,$port){
		echo "accept\n";
});

$server->bind('receive',function($fd,$message,$ip,$port){
		echo "receive\n";
});

$server->bind('close',function($fd,$ip,$port){
		echo "close\n";
});
*/
$server->run();

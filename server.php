<?php

$server = new php_server('192.168.1.113',9009);

print_r($server->get());

function bind_accept($fd,$ip,$port){
	echo "accept $fd $ip $port ]\n";
}

$server->bind('accept','bind_accept');

function bind_receive($fd,$message,$ip,$port){
	//$message = rtrim($message,"\r\n");
	echo "receive $fd $message $ip $port ]\n";
	echo $message;
	$response = <<<EOF
HTTP/1.1 200 OK
Server: bfe/1.0.5.38
Content-Type: text/html; charset=utf-8
Transfer-Encoding: chunked
Connection: close
Content-Length: 11

0123456789
EOF;
	php_server_send($fd,$response);
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

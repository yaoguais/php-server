<?php

$server = new php_server('192.168.1.104',9009);

print_r($server->get());

function bind_accept($fd,$ip,$port){
	echo "accept $fd $ip $port aaaaaaaaaaaaaaaaaaaaaaa]\n";
}

$server->bind('accept','bind_accept');

function bind_receive($fd,$message,$ip,$port){
	//$message = rtrim($message,"\r\n");
	//echo "receive $fd $message $ip $port ]\n";
	$messageLen = strlen($message);
	echo "receive $fd $messageLen $ip $port rrrrrrrrrrrrrrrrrrrr]\n";
	echo $message;
	$content = "<html><body><h2>PHP SERVER 1.0</h2></body></html>";
	$contentLen = strlen($content);
	$response = <<<EOF
HTTP/1.1 200 OK
Date: Tue, 17 Mar 2015 04:43:41 GMT
Server: PHP SERVER 1.0
X-Powered-By: PHP/7.0.0-dev
Content-Length: $contentLen
Connection: close
Content-Type: text/html; charset=UTF-8

$content
EOF;
	php_server_send($fd,$response,true);
	php_server_close($fd);
}

$server->bind('receive','bind_receive');

function bind_close($fd,$ip,$port){
	echo "close $fd $ip $port ccccccccccccccccccc]\n";
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

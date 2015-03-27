<?php

$server = new php_server('127.0.0.1',9009);

function bind_accept($fd,$ip,$port){
	
}

$server->bind('accept','bind_accept');

function bind_receive($fd,$message,$ip,$port){
	$content = "<h2>PHP SERVER 1.0</h2>";
	$contentLen = strlen($content);
	$dateStr = date('r');
	$response = <<<EOF
HTTP/1.1 200 OK
Date: $dateStr
Server: PHP SERVER 1.0
X-Powered-By: PHP/7.0.0-dev
Accept-Ranges: bytes
Content-Length: $contentLen
Connection: close
Content-Type: text/html

$content
EOF;
	php_server_send($fd,$response,false);
	php_server_close($fd);
}

$server->bind('receive','bind_receive');

function bind_close($fd,$ip,$port){
	
}

$server->bind('close','bind_close');
$server->run();

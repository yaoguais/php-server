<?php

$server = new php_server('127.0.0.1',9009);

//print_r($server->get());

function bind_accept($fd,$ip,$port){
	//echo "accept $fd $ip $port aaaaaaaaaaaaaaaaaaaaaaa]\n";
//	echo "accept ",microtime(true),"\n";
}

$server->bind('accept','bind_accept');

function bind_receive($fd,$message,$ip,$port){
	//$start = microtime(true);
	//$message = rtrim($message,"\r\n");
	//echo "receive $fd $message $ip $port ]\n";
	//$messageLen = strlen($message);
	//echo "receive $fd $messageLen $ip $port rrrrrrrrrrrrrrrrrrrr]\n";
	//echo $message;
	$content = "<html><body><h2>PHP SERVER 1.0</h2></body></html>";
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
	//$end = microtime(true);
	//echo "$start $end\n",($end - $start)*1000,"ms\n\n";
}

$server->bind('receive','bind_receive');

function bind_close($fd,$ip,$port){
	//echo "close $fd $ip $port ccccccccccccccccccc]\n";
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

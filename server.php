<?php
// path/to/php7/php server.php
// path/to/php7/php server.php enable-debug
define('ACCEPT','accept');
define('RECEIVE','receive');
define('CLOSE','close');
log_reset();

$ip = '127.0.0.1';
$port = 9009;
$server = new php_server($ip,$port);
/* test */
$settings = $server->get();
if( empty($settings) ||  $ip != $settings['ip'] || $port != $settings['port'] ){
	exit(__LINE__);
}
if( $ip != $server->get('ip') || $port != $server->get('port') ){
	exit(__LINE__);
}
$port2 = 9010;
$server->set('port',$port2);
if( $port2 != $server->get('port') ){
	exit(__LINE__);
}
$server->set('port',$port);
if( $port != $server->get('port') ){
	exit(__LINE__);
}
/* test end */
function bind_accept($fd,$ip,$port){
	user_log(ACCEPT);
}
$server->bind('accept','bind_accept');

function bind_receive($fd,$message,$ip,$port){
	user_log(RECEIVE);
	$content = "<h2>PHP SERVER 1.0</h2>";
	$contentLen = sizeof($content);
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
	user_log(CLOSE);
}
$server->bind('close','bind_close');
$server->run();
log_message();

function log_reset(){
	if(isset($_SERVER['argv'][1])){
		@unlink(__DIR__.'/test.out');
	}	
}

function user_log($msg){
	if(isset($_SERVER['argv'][1])){
		file_put_contents(__DIR__.'/test.out',$msg,FILE_APPEND);
	}
}

function log_message(){
	if(isset($_SERVER['argv'][1])){
		$content = file_get_contents(__DIR__.'/test.out');
		echo ACCEPT,': ',substr_count($content,ACCEPT),"\n";
		echo RECEIVE,': ',substr_count($content,RECEIVE),"\n";
		echo CLOSE,': ',substr_count($content,CLOSE),"\n";
	}
}
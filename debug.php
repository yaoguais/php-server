<?php

$dir = "/home/yaoguai/debug";

function get_files($dir){
	$files = array();
	$fos = opendir($dir);
	while($file = readdir($fos)){
		if($file == '.'  ||  $file == '..'){
			continue;
		}
		$files[] = $dir.'/'.$file;
	}
	return $files;
}

$files = get_files($dir);

function get_first_file(&$files){
	$min = 10000000;
	$fileKey = '';
	foreach ($files as $key=>$file){
		$pid = intval(str_replace(array('php-server-'),array(''),pathinfo($file,PATHINFO_FILENAME)));
		if($pid<$min){
			$min = $pid;
			$fileKey = $key;
		}
	}
	
	$ret = $files[$fileKey];
	unset($files[$fileKey]);
	return $ret;
}

//$firstFile = get_first_file($files);
//$keywords = array('__will_accept','epoll come in master:0','epoll come in master:1','master_break:0','master_break:1');

function debug_file($file,$keywords){
	echo pathinfo($file,PATHINFO_FILENAME),"\n";
	$string = file_get_contents($file);
	foreach ($keywords as $keyword){
		echo $keyword,': ',substr_count($string,$keyword),"\n";
	}
	echo "\n";
}

//debug_file($firstFile,$keywords);

$keywords = array('__accepted','__accept_error','__recv_error','__recv_from','__recv_once','__send','__close','__client_close');

foreach ($files as $file){
	debug_file($file,$keywords);
}
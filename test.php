<?php

$obj = new php_server('0.0.0.0',9000);

var_dump($obj->get());

var_dump($obj->get('port'));

var_dump($obj->set('ip','127.0.0.1'));

var_dump($obj->get('ip'));

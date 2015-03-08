<?php

/**
 * Class php_server
 */
class php_server{

    /**
     * @var array 当前的配置
     */
    private $_settings = array();

    /**
     * 架构函数
     * @param string $ip IP地址
     * @param int $port 端口
     * function php_server_construct
     */
    public function __construct($ip,$port){

        $this->_settings['ip'] = $ip;
        $this->_settings['port'] = $port;
    }

    /**
     * 事件回调函数
     * @param string $event
     * @param function $callback
     * @return bool
     * function php_server_bind
     */
    public function bind($event,$callback){

        return true;
    }

    /**
     * 向客户端发送数据
     * @param $message
     * @return bool
     * function php_server_send
     */
    public function send($message){

        return true;
    }

    /**
     * 设置当前的配置
     * @param string $key
     * @param mixed $value
     * function php_server_set
     */
    public function set($key,$value){

        $this->_settings[$key] = $value;
    }

    /**
     * 获取当前的配置
     * @param string|null $key
     * @return string
     * function php_server_get
     */
    public function get($key){

        return is_string($key) ? $this->_settings[$key] : $this->_settings;
    }


    /**
     * 启动服务器
     * @return bool
     * function php_server_run
     */
    public function run(){

    }
}


/**
 * 接受连接时的回调
 * @param array $remote = array( 'ip' => '' , port => 0 )
 * @return void
 */
function bind_accept($remote){

}


/**
 * 接受消息时的回调
 * @param string $message
 * @param array $remote
 */
function bind_receive($message,$remote){

}

/**
 * 当连接关闭时的回调
 * @param array $remote
 */
function bind_close($remote){

}
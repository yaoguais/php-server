# php-server #

这是一个可以作为TCP服务器的PHP扩展。

配置项有：

1. 进程池的大小process\_number
2. 进程的名称master\_name、worker\_name


扩展提供的类与全局函数：

	class php_server{
	
	    /**
	     * @var array 当前的配置
	     */
	    private $settings = array();
	
	    /**
	     * 架构函数
	     * @param string $ip IP地址
	     * @param int $port 端口
	     * function php_server_construct
	     */
	    public function __construct($ip,$port){
	
	        $this->settings['ip'] = $ip;
	        $this->settings['port'] = $port;
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
	
	        $this->settings[$key] = $value;
	    }
	
	    /**
	     * 获取当前的配置
	     * @param string|null $key
	     * @return string
	     * function php_server_get
	     */
	    public function get($key){
	
	        return is_string($key) ? $this->settings[$key] : $this->settings;
	    }
	
	
	    /**
	     * 启动服务器
	     * @return bool
	     * function php_server_run
	     */
	    public function run(){
	
	        return true;
	    }
	}


事件绑定的回调函数：

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


信号处理：

可以向master、worker发送SIGTERM或SIGINT信号终止该进程

服务端首先启动进程池，监听客户连接的到来

![服务端进程](https://raw.githubusercontent.com/Yaoguais/php-server/master/images/server.png)


杀死主进程后，主进程收到SIGTERM信号,然后主进程通知所有子进程退出。子进程全部退出后，主进程最后退出。

![向主进程发送终止信号](https://raw.githubusercontent.com/Yaoguais/php-server/master/images/kill.png)
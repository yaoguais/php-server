# php-server #

这是一个可以作为TCP服务器的PHP扩展。

1. 配置选项
2. 类与函数
3. 安装扩展
3. 信号处理
4. 压力测试


配置项有：

1. 进程池的大小process\_number
2. 进程的名称master\_name、worker\_name
3. 是否开启调试模式debug


扩展提供的类与全局函数：

	class php_server{
	
	    /**
	     * @var array 当前的配置
	     */
	    private $_settings = array();
	
	    /**
	     * 架构函数
	     * @param string $ip IP地址
	     * @param int $port 端口
	     * function php_server_create
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
	
	        return true;
	    }
	}


事件绑定的回调函数：

	/**
	 * 接受连接时的回调
	 * @param int $fd
	 * @param string $ip
	 * @param int $port
	 * @return void
	 */
	function bind_accept($fd,$ip,$port){
	
	}
	
	
	/**
	 * 接受消息时的回调
	 * @param int $fd
	 * @param string $message
	 * @param string $ip
	 * @param int $port
	 */
	function bind_receive($fd,$message,$ip,$port){
	
	}
	
	/**
	 * 当连接关闭时的回调
	 * @param int $fd
	 * @param string $ip
	 * @param int $port
	 */
	function bind_close($fd,$ip,$port){
	
	}

全局函数：

	/**
	 * 主动向客户端发送消息
	 * @param int $fd
	 * @param string $message
	 * @param bool $flush
	 * @return int (成功发送的字节数)
	 */
	function php_server_send($fd,$message,$flush=true){

	}

	/**
	 * 主动关闭客户端连接
	 * @param int $fd
	 */
	function php_server_close($fd){

	}


安装扩展：

	phpize
	./configure
	make && make install
	
	编辑php.ini并添加
	extension = php_server.so
	php_server.process_number = 7
	php_server.master_name 	  = "php server master"
	php_server.worker_name 	  = "php server worker"
	php_server.debug		  =	Off
	
	执行php php_server.php即可启动服务器

信号处理：

可以向master、worker发送SIGTERM或SIGINT信号终止该进程

也可以直接CRTL+C直接结束进程池

服务端首先启动进程池，监听客户连接的到来

杀死主进程后，主进程收到SIGTERM信号,然后主进程通知所有子进程退出。子进程全部退出后，主进程最后退出。


ApacheBench：

	PHP Version			: 7.0.0-dev
	Linux version		: Linux version 3.13.0-34-generic 
						  (buildd@allspice) (gcc version 4.8.2 (Ubuntu 4.8.2-19ubuntu1) )
	内存					: 1GB
	处理器				: 1
	http content		: <h2>PHP SERVER 1.0</h2>
	content length		: 24 bytes
	php server version	: 1.0
	Apache2 version		: Apache/2.4.7 (Ubuntu)

经测试，PHP-SERVER rps一直在30,000以上,apache2-server rps也在30,000以上。

![php-server](https://raw.githubusercontent.com/Yaoguais/php-server/master/images/php-server.png)

![apache-server](https://raw.githubusercontent.com/Yaoguais/php-server/master/images/apache-server.png)

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


安装扩展：

	phpize
	./configure
	make && make install
	
	编辑php.ini并添加
	extension = php_server.so
	
	执行php php_server.php即可启动服务器

信号处理：

可以向master、worker发送SIGTERM或SIGINT信号终止该进程

服务端首先启动进程池，监听客户连接的到来

![服务端进程](https://raw.githubusercontent.com/Yaoguais/php-server/master/images/server.png)


杀死主进程后，主进程收到SIGTERM信号,然后主进程通知所有子进程退出。子进程全部退出后，主进程最后退出。

![向主进程发送终止信号](https://raw.githubusercontent.com/Yaoguais/php-server/master/images/kill.png)


ApacheBench：

	VMware® Workstation : 10.0.1 build-1379776
	PHP Version			: 7.0.0-dev
	Linux version		: 2.6.32-504.1.3.el6.x86_64 (gcc version 4.4.7 20120313 (Red Hat 4.4.7-11) (GCC) )
	内存					: 1GB
	处理器				: 1
	http content		: 0123456789

经测试，10次中有8次在3000以上，有两次会急剧减低到800多。

![ab](https://raw.githubusercontent.com/Yaoguais/php-server/master/images/bench.png)

# PHP SERVER #

这是一个可以作为TCP服务器的PHP扩展(适用7.0.0RC1),性能跟Apache不相上下,在单机4G内存4核上rqs都在3万以上。

1. [配置选项](https://github.com/Yaoguais/php-server#配置选项)
2. [类与函数](https://github.com/Yaoguais/php-server#类与函数)
3. [安装扩展](https://github.com/Yaoguais/php-server#安装扩展)
4. [信号处理](https://github.com/Yaoguais/php-server#信号处理)
5. [调试程序](https://github.com/Yaoguais/php-server#调试程序)
6. [压力测试](https://github.com/Yaoguais/php-server#压力测试)


#配置选项#

1. 进程的名称master\_name
2. 是否开启调试模式debug
3. 是否开启调试输出文件debug_file

详细说明

	master_name:
	字符串，如 php_server.master_name = "php server"

	debug:
	布尔值，如 php_server.debug = On
			  php_server.debug = Off

	debug_file:
	字符串，如 php_server.debug_file = "/home/yaoguai/php-server-%d.txt"
	其中%d是进程号的占位符
	
#类与函数#

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
	    public function get($key=null){
	
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


#安装扩展#

	phpize
	./configure
	make && make install
	
	编辑php.ini并添加
	[php_server]
	extension = php_server.so
	php_server.master_name 	  = "php server"
	php_server.debug		  =	Off
	php_server.debug_file     = ""
	
	执行php server.php即可启动服务器

#信号处理#

可以向进程发送SIGTERM或SIGINT信号终止该进程,这样可以让应用处理完所有的业务才自动终止。

也可以直接CRTL+C直接结束进程池。


#调试程序#

多进程多线程程序本来就很难调试，再加上高并发，单步只能调试一般的错误，而有些错误只在高并发下才会有，
所以我这里采用的是通过调试文件记录关键字来调试程序。其实现为debug.php。

用法：

	;修改php.ini
	;添加
	php_server.debug = On
	php_server.debug_file = "/home/yaoguai/php-server-%d.txt"

	#运行服务器程序
	/root/php7d/bin/php server.php
	#打开另一个终端
	ab -c 100 -n 1000 http://127.0.0.1:9009/
	#在第一个的终端CTRL+C结束server.php
	#运行结果收集程序
	/root/php7d/bin/php debug.php
	#屏幕会显示关于接收连接、接收数据、发送数据、关闭连接的统计

	#在脚本中收集调试信息(上面是扩展中的内部收集数据)
	/root/php7d/bin/php server.php enable-debug
	#ctrl+C 结束进程屏幕会显示accept,receive,close的统计数量

在开启调试模式的时候，rqs一直在8000左右，只有关闭调试的四分之一。
	
#压力测试#

压力测试采用Apache的ab工具进行，100的并发，1000的连接。

	ApacheBench：
	ab -c 100 -n 1000 http://127.0.0.1:9009/

	php server测试server.php
	apache测试index.html
	PHP Version:7.0.0-dev
	Linux version:Linux version 3.13.0-34-generic (buildd@allspice) (gcc version 4.8.2 (Ubuntu 4.8.2-19ubuntu1) )
	内存	:3.8 GiB
	处理器:Intel® Core™ i5-4430 CPU @ 3.00GHz × 4
	http content:<h2>PHP SERVER 1.0</h2>
	content length:24 bytes
	php server version:1.0
	Apache2 version:Apache/2.4.7 (Ubuntu)

经测试，PHP-SERVER rps一直在30,000以上,apache2-server rps也在30,000以上。
/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PHP_SERVER_H
#define PHP_PHP_SERVER_H

extern zend_module_entry php_server_module_entry;
#define phpext_php_server_ptr &php_server_module_entry

#define PHP_PHP_SERVER_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_PHP_SERVER_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_PHP_SERVER_API __attribute__ ((visibility("default")))
#else
#	define PHP_PHP_SERVER_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif


ZEND_BEGIN_MODULE_GLOBALS(php_server)
	int process_number;
	char * master_name;
	char * worker_name;
ZEND_END_MODULE_GLOBALS(php_server)


/* 定义存储进程信息的结构体 */
typedef struct php_server_process{
	//当前子进程的数量
	unsigned int	process_number;		
	//当前进程的序号,父进程为-1,子进程从0开始
	int		process_index;
	//子进程的PID
	pid_t * child_pid;
	//epoll事件表标识
	int epoll_fd;
	//整个应用监听的sokect
	int socket_fd;
	//当前进程是否停止运行
	zend_bool is_stop;
	//用作进程间通讯的双端管道
	int ** pipe_fd;	
}server_process;

/* function declare*/
PHP_FUNCTION(php_server_create);
PHP_FUNCTION(php_server_bind);
PHP_FUNCTION(php_server_send);
PHP_FUNCTION(php_server_set);
PHP_FUNCTION(php_server_get);
PHP_FUNCTION(php_server_run);
void php_server_set_proc_name(int argc,char ** argv,char * name);
void php_set_proc_name(char * name);
int php_server_set_nonblock(int fd);
void php_server_epoll_add_read_fd(int epoll_fd,int fd);
void php_server_epoll_del_fd(int epoll_fd,int fd);
void php_server_epoll_remove_fd(int epoll_fd,int fd);
zend_bool php_server_setup_socket(char * ip,int port);
zend_bool php_server_shutdown_socket();
zend_bool php_server_setup_process_pool(int socket_fd,	unsigned int process_number);
void php_server_sig_handler(int signal_no);
zend_bool php_server_run_init();
zend_bool php_server_clear_init();
int php_server_run_master_process();
int php_server_recv_from_client(int sock_fd);
int php_server_run_worker_process();
void php_server_run();
zend_bool php_server_shutdown_process_pool(unsigned int process_number);


/* Always refer to the globals in your function as PHP_SERVER_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define PHP_SERVER_G(v) ZEND_TSRMG(php_server_globals_id, zend_php_server_globals *, v)
#ifdef COMPILE_DL_PHP_SERVER
ZEND_TSRMLS_CACHE_EXTERN;
#endif
#else
#define PHP_SERVER_G(v) (php_server_globals.v)
#endif

#endif	/* PHP_PHP_SERVER_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

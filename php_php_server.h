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
  | Author: Yaoguai (newtopstdio@163.com)                                |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_PHP_SERVER_H
#define PHP_PHP_SERVER_H

extern zend_module_entry php_server_module_entry;
#define phpext_php_server_ptr &php_server_module_entry

// fix 7.0.0RC1
#define PHP_PHP_SERVER_VERSION "0.1.1"

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
	char * master_name;
	int debug;
	char * debug_file;
ZEND_END_MODULE_GLOBALS(php_server)


typedef struct php_server_process{
	int epoll_fd;
	int socket_fd;
	int is_stop;
}server_process;

/* function declare */
PHP_FUNCTION(php_server_create);
PHP_FUNCTION(php_server_bind);
PHP_FUNCTION(php_server_send);
PHP_FUNCTION(php_server_close);
PHP_FUNCTION(php_server_set);
PHP_FUNCTION(php_server_get);
PHP_FUNCTION(php_server_run);
void php_set_proc_name(char * name);
void php_server_set_debug_file();
int php_server_set_nonblock(int fd);
void php_server_epoll_add_read_fd(int epoll_fd,int fd,uint32_t events);
int php_server_epoll_del_fd(int epoll_fd,int fd);
zend_bool php_server_setup_socket(char * ip,int port);
zend_bool php_server_shutdown_socket();
void php_server_sig_handler(int signal_no);
zend_bool php_server_run_init();
zend_bool php_server_clear_init();
void php_server_epoll_debug(char * tag,uint32_t events);
zend_bool php_server_run_process();
zend_bool php_server_accept_client();
zend_bool php_server_close_client(int sock_fd);
int php_server_recv_from_client(int sock_fd);

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

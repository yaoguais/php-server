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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "php_main.h"
#include "ext/standard/info.h"
#include "php_php_server.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <malloc.h>
#include <time.h>

static zend_php_server_globals php_server_globals;

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("php_server.master_name", "php server master", PHP_INI_ALL, OnUpdateString, master_name, zend_php_server_globals, php_server_globals)
	STD_PHP_INI_ENTRY("php_server.debug", "0", PHP_INI_ALL, OnUpdateBool, debug, zend_php_server_globals, php_server_globals)
	STD_PHP_INI_ENTRY("php_server.debug_file", "", PHP_INI_ALL, OnUpdateString, debug_file, zend_php_server_globals, php_server_globals)
PHP_INI_END()

#define MAX_EPOLL_NUMBER 10000
#define PHP_SERVER_DEBUG(format,...) do{ \
											if(php_server_globals.debug)		\
												printf(format, ##__VA_ARGS__);	\
									   }while(0);

//no need for thread safe
#define BUFFER_SIZE 8096
static char recv_buffer[BUFFER_SIZE];
static int socket_fd_global;
static server_process *  process_global;
static zend_class_entry * php_server_class_entry;
static HashTable callback_ht;

ZEND_BEGIN_ARG_INFO_EX(arginfo_php_server_create, 0, 0, 2)
	ZEND_ARG_INFO(0,ip)
	ZEND_ARG_INFO(0,port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_php_server_bind, 0, 0, 2)
	ZEND_ARG_INFO(0,event)
	ZEND_ARG_INFO(0,callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_php_server_send, 0, 0, 2)
	ZEND_ARG_INFO(0,sockfd)
	ZEND_ARG_INFO(0,message)
	ZEND_ARG_INFO(0,flush)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_php_server_set, 0, 0, 2)
	ZEND_ARG_INFO(0,key)
	ZEND_ARG_INFO(0,value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_php_server_get, 0, 0, 0)
	ZEND_ARG_INFO(0,key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_php_server_close, 0, 0, 1)
	ZEND_ARG_INFO(0,sockfd)
ZEND_END_ARG_INFO()


const zend_function_entry php_server_class_functions[] = {
		ZEND_FENTRY(__construct,PHP_FN(php_server_create),arginfo_php_server_create,ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
		ZEND_FENTRY(bind,PHP_FN(php_server_bind),arginfo_php_server_bind,ZEND_ACC_PUBLIC)
		ZEND_FENTRY(set,PHP_FN(php_server_set),arginfo_php_server_set,ZEND_ACC_PUBLIC)
		ZEND_FENTRY(get,PHP_FN(php_server_get),arginfo_php_server_get,ZEND_ACC_PUBLIC)
		ZEND_FENTRY(run,PHP_FN(php_server_run),NULL,ZEND_ACC_PUBLIC)
		PHP_FE_END
};

PHP_FUNCTION(php_server_create)
{
	char * ip;
	size_t ip_len;
	zend_long port;

#ifdef FAST_ZPP
	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STRING(ip,ip_len)
	Z_PARAM_LONG(port)
	ZEND_PARSE_PARAMETERS_END();
#else
	if(zend_parse_parameters(ZEND_NUM_ARGS(),"sl",&ip,&ip_len,&port) == FAILURE){
		return;
	}
#endif

	zval * this_settings = zend_read_property(php_server_class_entry,getThis(),"_settings",sizeof("_settings")-1,0,NULL);
	array_init(this_settings);
	add_assoc_stringl_ex(this_settings,"ip",sizeof("ip")-1,ip,ip_len);
	add_assoc_long_ex(this_settings,"port",sizeof("port")-1,(zend_long)port);
	zend_update_property(php_server_class_entry,getThis(),"_settings",sizeof("_settings")-1,this_settings);
}

PHP_FUNCTION(php_server_bind)
{
	zval * event , *callback;

#ifdef FAST_ZPP
	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_ZVAL(event)
	Z_PARAM_ZVAL(callback)
	ZEND_PARSE_PARAMETERS_END();
#else
	if(zend_parse_parameters(ZEND_NUM_ARGS(),"zz",&event,&callback) == FAILURE || Z_TYPE_P(event) != IS_STRING){
		return;
	}
#endif

	zend_hash_add(&callback_ht,Z_STR_P(event),callback);
	zval_ptr_dtor(event);
	zval_ptr_dtor(callback);
}

PHP_FUNCTION(php_server_send) {
	char * message;
	size_t message_len;
	zend_long sockfd;
	zend_bool is_flush = 0;
	int ret = -1;
	FILE * fp;

#ifdef FAST_ZPP
	ZEND_PARSE_PARAMETERS_START(2,3)
	Z_PARAM_LONG(sockfd)
	Z_PARAM_STRING(message,message_len)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(is_flush)
	ZEND_PARSE_PARAMETERS_END();
#else
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ls|b", &sockfd, &message,
				&message_len, &is_flush) != FAILURE) {
		return;
	}
#endif

	ret = send((int) sockfd, message, message_len, 0);
	if (is_flush) {
		if (NULL != (fp = fdopen(sockfd, "rw"))) {
			fflush(fp);
			fp = NULL;
			PHP_SERVER_DEBUG("__flush %d size\n", ret);
		}
	}
	PHP_SERVER_DEBUG("__send %d\n", (int )sockfd);

	RETURN_LONG(ret);
}

PHP_FUNCTION(php_server_close)
{
	zend_long sockfd;

#ifdef FAST_ZPP
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(sockfd)
	ZEND_PARSE_PARAMETERS_END();
#else
	if(zend_parse_parameters(ZEND_NUM_ARGS(),"l",&sockfd) == FAILURE){
		return;
	}
#endif

	php_server_close_client(sockfd);
	PHP_SERVER_DEBUG("__close sockfd %d\n",(int)sockfd);
}

PHP_FUNCTION(php_server_set)
{
	char * key_str;
	size_t key_len;
	zval *value;

#ifdef FAST_APP
	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STRING(key_str,key_len)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();
#else
	if(zend_parse_parameters(ZEND_NUM_ARGS(),"sz",&key_str,&key_len,&value) == FAILURE){
		return;
	}
#endif

	zval * this_settings = zend_read_property(php_server_class_entry,getThis(),"_settings",sizeof("_settings")-1,0,NULL);
	zend_hash_str_update(Z_ARRVAL_P(this_settings),key_str,key_len,value);
}

PHP_FUNCTION(php_server_get)
{
	char * key = NULL;
	size_t key_len;

#ifdef FAST_APP
	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_STRING(key,key_len)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();
#else
	if(zend_parse_parameters(ZEND_NUM_ARGS(),"|s",&key,&key_len) == FAILURE){
		return;
	}
#endif

	zval * this_settings = zend_read_property(php_server_class_entry,getThis(),"_settings",sizeof("_settings")-1,0,NULL);
	if(0 == ZEND_NUM_ARGS()){
		RETURN_ZVAL(this_settings,1,NULL);
	}else{
		zval * retval = zend_hash_str_find(Z_ARRVAL_P(this_settings),key,key_len);
		if(retval){
			RETURN_ZVAL(retval,1,NULL);
		}
	}
}

PHP_FUNCTION(php_server_run){
	zval * this_settings = zend_read_property(php_server_class_entry,getThis(),"_settings",sizeof("_settings")-1,0,NULL);
	zval * ip = zend_hash_str_find(Z_ARRVAL_P(this_settings),"ip",sizeof("ip")-1);
	zval * port = zend_hash_str_find(Z_ARRVAL_P(this_settings),"port",sizeof("port")-1);
	if(Z_TYPE_P(ip)!= IS_STRING  || Z_TYPE_P(port)!=IS_LONG){
		php_error_docref(NULL,E_ERROR,"ip must be string and port must be int");
		return;
	}

	zend_bool ret = php_server_setup_socket(Z_STRVAL_P(ip),(int)Z_LVAL_P(port));
	PHP_SERVER_DEBUG("setup_socket:%d\n",ret);
	if(ret!=SUCCESS){
		php_error_docref(NULL,E_ERROR,"create socket error\n");
		return;
	}

	process_global = (server_process * ) malloc(sizeof(server_process));
	process_global->socket_fd = socket_fd_global;
	php_set_proc_name(PHP_SERVER_G(master_name));
	php_server_run_process();
	free(process_global);
	php_server_shutdown_socket();
}

static void php_php_server_init_globals(zend_php_server_globals *php_server_globals)
{
	php_server_globals->master_name = NULL;
	php_server_globals->debug = 0;
	php_server_globals->debug_file = "";
}

PHP_MINIT_FUNCTION(php_server)
{
	REGISTER_INI_ENTRIES();

	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"php_server",php_server_class_functions);
	php_server_class_entry = zend_register_internal_class(&ce);
	zend_declare_property_null(php_server_class_entry,"_settings",sizeof("_settings")-1,ZEND_ACC_PUBLIC);
	zend_hash_init(&callback_ht, 0, NULL, NULL, 1);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(php_server)
{
	UNREGISTER_INI_ENTRIES();
	zend_hash_destroy(&callback_ht);
	return SUCCESS;
}

PHP_RINIT_FUNCTION(php_server)
{
#if defined(COMPILE_DL_PHP_SERVER) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE;
#endif
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(php_server)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(php_server)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "php_server support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

void php_set_proc_name(char * name){
	zval argv[1];
	zval retval;
	zval function_name;
	ZVAL_STRING(&function_name,"cli_set_process_title");
	ZVAL_STRING(&argv[0],name);
	if(call_user_function_ex(EG(function_table),NULL,&function_name,&retval,1,argv,0,NULL) == FAILURE){
		php_error_docref(NULL, E_WARNING, "Could not call the cli_set_process_name");
	}
	zval_ptr_dtor(&argv[0]);
	zval_ptr_dtor(&function_name);
}

void php_server_set_debug_file(){
	int fileno;
	char fileBuf[200];
	snprintf(fileBuf,199,php_server_globals.debug_file,getpid());
	if(strcmp(php_server_globals.debug_file,"")!=0){
		fileno = open(fileBuf,O_RDWR|O_CREAT, S_IRUSR|S_IWUSR);
		if(fileno>=0){
			if(dup2(fileno,STDOUT_FILENO)<0){
				PHP_SERVER_DEBUG("dup2 error\n");
			}
			close(fileno);
		}else{
			PHP_SERVER_DEBUG("out fileno open error\n");
		}
	}
}

int php_server_set_nonblock(int fd){
	int old_option,new_option;
	old_option = fcntl(fd,F_GETFL);
	new_option = old_option | O_NONBLOCK;
	fcntl(fd,F_SETFL,new_option);
	return old_option;
}

void php_server_epoll_add_read_fd(int epoll_fd,int fd,uint32_t events){
	struct epoll_event event;
	event.data.fd = fd;
	event.events = events;
	epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd, &event);
}

int php_server_epoll_del_fd(int epoll_fd,int fd){
	int ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL,fd,0);
	close(fd);
	return ret;
}

zend_bool php_server_setup_socket(char * ip,int port){
	int ret;
	struct sockaddr_in address;
	
	socket_fd_global = socket(PF_INET,SOCK_STREAM,0);
	if(socket_fd_global<0){
		PHP_SERVER_DEBUG("socket error\n");
		return FAILURE;
	}
	bzero(&address,sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET,ip,&address.sin_addr);
	address.sin_port = htons(port);
	ret = bind(socket_fd_global,(struct sockaddr * )&address,sizeof(address));
	if(-1 == ret){
		PHP_SERVER_DEBUG("bind error\n");
		return FAILURE;
	}
	ret = listen(socket_fd_global,SOMAXCONN);
	if(-1 == ret){
		PHP_SERVER_DEBUG("listen error\n");
		return FAILURE;
	}
	return SUCCESS;
}

zend_bool php_server_shutdown_socket(){
	close(socket_fd_global);
	return SUCCESS;
}

void php_server_sig_handler(int signal_no){
	switch(signal_no){
		case SIGTERM:
		case SIGINT:
		process_global->is_stop = 1;
		break;
	}
}

zend_bool php_server_run_init(){
	php_server_set_debug_file();
	process_global->epoll_fd = epoll_create(MAX_EPOLL_NUMBER);
	if(process_global->epoll_fd == -1){
		return FAILURE;
	}
	process_global->is_stop = 0;
	signal(SIGTERM,php_server_sig_handler);
	signal(SIGINT,php_server_sig_handler);
	return SUCCESS;
}

zend_bool php_server_clear_init(){
	close(process_global->epoll_fd);
	return SUCCESS;
}

int php_server_run_process(){
	struct epoll_event events[MAX_EPOLL_NUMBER];
	int i,number,ret,sock_fd;
	php_server_run_init();
	php_server_set_nonblock(process_global->socket_fd);
	php_server_epoll_add_read_fd(process_global->epoll_fd, process_global->socket_fd,EPOLLIN);
	while(!process_global->is_stop){
		number = epoll_wait(process_global->epoll_fd,events,MAX_EPOLL_NUMBER,-1);
		PHP_SERVER_DEBUG("epoll come in worker:%d worker_break:%d pid(%d)\n",number,number<0 && errno!=EINTR,getpid());
		if(number<0 && errno != EINTR){
			break;
		}
		for(i=0;i<number;i++){
			sock_fd = events[i].data.fd;
			if(events[i].events & EPOLLIN){
				if(sock_fd == process_global->socket_fd){
					php_server_accept_client();
				}else{
					php_server_recv_from_client(sock_fd);
				}
			}
		}
	}
	php_server_clear_init();
	return 0;
}

int php_server_accept_client(){
	int conn_fd;
	struct sockaddr_in client;
	socklen_t client_len = sizeof(client);
	bzero(&client,client_len);
	conn_fd = accept(process_global->socket_fd,(struct sockaddr *) & client, &client_len);
	if(conn_fd < 0){
		PHP_SERVER_DEBUG("worker(%d) __accept_error %d\n",getpid(),conn_fd);
		return -1;
	}
	PHP_SERVER_DEBUG("worker(%d) __accepted %d\n",getpid(),conn_fd);
	php_server_set_nonblock(conn_fd);
	php_server_epoll_add_read_fd(process_global->epoll_fd,conn_fd,EPOLLIN | EPOLLET);
	/* call accept */
	zval * accept_cb = zend_hash_str_find(&callback_ht,"accept",sizeof("accept")-1);
	char client_ip_str[INET_ADDRSTRLEN];
	long port = (long)ntohs(client.sin_port);
	if(NULL!=accept_cb && port > 0 && NULL != inet_ntop(AF_INET,(void *)&client.sin_addr.s_addr,client_ip_str,INET_ADDRSTRLEN)){
		zval args[3], retval;
		ZVAL_LONG(&args[0],conn_fd);
		ZVAL_STRING(&args[1],client_ip_str);
		ZVAL_LONG(&args[2],port);
		if(call_user_function_ex(EG(function_table),NULL,accept_cb,&retval,3,args,0,NULL)){
			php_error_docref(NULL,E_WARNING,"accept error.\n");
		}
		PHP_SERVER_DEBUG("worker(%d) accept %d after call\n",getpid(),conn_fd);
		zval_dtor(&args[0]);
		zval_dtor(&args[1]);
		zval_dtor(&args[2]);
	}
	/* call accept end */
	return 0;
}

int php_server_close_client(int sock_fd){
	if(php_server_epoll_del_fd(process_global->epoll_fd,sock_fd)<0){
		return -1;
	}
	/* call close */
	struct sockaddr_in client;
	size_t client_len = sizeof(client);
	getpeername(sock_fd,(struct sockaddr *)&client,(socklen_t *)&client_len);
	zval * close_cb = zend_hash_str_find(&callback_ht,"close",sizeof("close")-1);
	char client_ip_str[INET_ADDRSTRLEN];
	long port = (long)ntohs(client.sin_port);
	if(NULL!=close_cb && port > 0 && NULL != inet_ntop(AF_INET,(void *)&client.sin_addr.s_addr,client_ip_str,INET_ADDRSTRLEN)){
		zval args[3], retval;
		ZVAL_LONG(&args[0],sock_fd);
		ZVAL_STRING(&args[1],client_ip_str);
		ZVAL_LONG(&args[2],port);
		if(call_user_function_ex(EG(function_table),NULL,close_cb,&retval,3,args,0,NULL)){
			php_error_docref(NULL,E_WARNING,"close error.\n");
		}
		zval_dtor(&args[0]);
		zval_dtor(&args[1]);
		zval_dtor(&args[2]);
	}
	/* call close end */
	return 0;
}

int php_server_recv_from_client(int sock_fd){
	int ret,length = 0;
	for(;;){
		bzero(recv_buffer,sizeof(recv_buffer));
		ret = recv(sock_fd,recv_buffer,sizeof(recv_buffer),0);
		if(ret<0){
			if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN){
				PHP_SERVER_DEBUG("worker __recv_once\n");
			}else{
				if(php_server_close_client(sock_fd) == 0){
					PHP_SERVER_DEBUG("worker __recv_error\n");
				}
			}
			break;
		}else if(ret == 0){
			PHP_SERVER_DEBUG("worker %d __client_close after call\n",sock_fd);
			php_server_close_client(sock_fd);
			break;
		}else{
			length += ret;
			PHP_SERVER_DEBUG("worker(%d) __recv_from %d:\n%s\n",getpid(),sock_fd,recv_buffer);
			/* call receive */
			struct sockaddr_in client;
			size_t client_len = sizeof(client);
			getpeername(sock_fd,(struct sockaddr *)&client,(socklen_t *)&client_len);
			zval * receive_cb = zend_hash_str_find(&callback_ht,"receive",sizeof("receive")-1);
			char client_ip_str[INET_ADDRSTRLEN];
			long port = (long)ntohs(client.sin_port);
			if(NULL!=receive_cb && port > 0 && NULL != inet_ntop(AF_INET,(void *)&client.sin_addr.s_addr,client_ip_str,INET_ADDRSTRLEN)){
				zval args[4], retval;
				ZVAL_LONG(&args[0],sock_fd);
				ZVAL_STRINGL(&args[1],recv_buffer,ret);
				ZVAL_STRING(&args[2],client_ip_str);
				ZVAL_LONG(&args[3],port);
				if(call_user_function_ex(EG(function_table),NULL,receive_cb,&retval,4,args,0,NULL)){
					php_error_docref(NULL,E_WARNING,"receive error.\n");
				}
				PHP_SERVER_DEBUG("worker(%d) receive %d after call\n",getpid(),sock_fd);
				zval_dtor(&args[0]);
				zval_dtor(&args[1]);
				zval_dtor(&args[2]);
				zval_dtor(&args[3]);
			}
			/* call receive end */
		}
	}
	return length;
}

const zend_function_entry php_server_functions[] = {
	PHP_FE(php_server_send,arginfo_php_server_send)
	PHP_FE(php_server_close,arginfo_php_server_close)
	PHP_FE_END
};

zend_module_entry php_server_module_entry = {
	STANDARD_MODULE_HEADER,
	"php_server",
	php_server_functions,
	PHP_MINIT(php_server),
	PHP_MSHUTDOWN(php_server),
	PHP_RINIT(php_server),
	PHP_RSHUTDOWN(php_server),
	PHP_MINFO(php_server),
	PHP_PHP_SERVER_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PHP_SERVER
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE;
#endif
ZEND_GET_MODULE(php_server)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

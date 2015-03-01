dnl $Id$
dnl config.m4 for extension php_server

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(php_server, for php_server support,
dnl Make sure that the comment is aligned:
dnl [  --with-php_server             Include php_server support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(php_server, whether to enable php_server support,
dnl Make sure that the comment is aligned:
dnl [  --enable-php_server           Enable php_server support])

if test "$PHP_PHP_SERVER" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-php_server -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/php_server.h"  # you most likely want to change this
  dnl if test -r $PHP_PHP_SERVER/$SEARCH_FOR; then # path given as parameter
  dnl   PHP_SERVER_DIR=$PHP_PHP_SERVER
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for php_server files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       PHP_SERVER_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$PHP_SERVER_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the php_server distribution])
  dnl fi

  dnl # --with-php_server -> add include path
  dnl PHP_ADD_INCLUDE($PHP_SERVER_DIR/include)

  dnl # --with-php_server -> check for lib and symbol presence
  dnl LIBNAME=php_server # you may want to change this
  dnl LIBSYMBOL=php_server # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PHP_SERVER_DIR/$PHP_LIBDIR, PHP_SERVER_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_PHP_SERVERLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong php_server lib version or lib not found])
  dnl ],[
  dnl   -L$PHP_SERVER_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(PHP_SERVER_SHARED_LIBADD)

  PHP_NEW_EXTENSION(php_server, php_server.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi

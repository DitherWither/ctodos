AC_INIT([ctodos], [0.1], [vardhanpatil@proton.me])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_FILES([
    Makefile
    src/Makefile    
])
AC_CONFIG_HEADERS([config.h])

AC_DEFINE([SERVER_PORT], [8080], [Port the server runs on])
AC_DEFINE([REQUEST_MAX_SIZE], [8192], [Max size for http request])
AC_DEFINE([RESPONSE_MAX_SIZE], [8192], [Max size for http response])

AC_OUTPUT
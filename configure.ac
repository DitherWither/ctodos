AC_INIT([ctodos], [0.1], [vardhanpatil@proton.me])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_FILES([
    Makefile
    src/Makefile    
])
AC_CONFIG_HEADERS([config.h])

AC_DEFINE([SERVER_PORT], [8080], [Port the server runs on])
AC_DEFINE([HTTP_REQUEST_MAX_SIZE], [8192], [Max size for http request])
AC_DEFINE([HTTP_RESPONSE_MAX_SIZE], [8192], [Max size for http response])
AC_DEFINE([HTTP_PATH_MAX_SIZE], [256], [Max size for the path in http])
AC_DEFINE([HTTP_MAX_HEADERS], [64], [Max size for the path in http])
AC_DEFINE([HTTP_MAX_BODY_SIZE], [4096], [Max size of the http body])

AC_ARG_ENABLE(
    [ipv6],
    AS_HELP_STRING([--disable-ipv6], [Disable ipv6])
)

AS_IF(
    [test "x$enable_ipv6" = "xno"], [
        AC_DEFINE([USE_IPV4], [1], [Use ipv4 and disable ipv6])
    ]
)

AC_OUTPUT
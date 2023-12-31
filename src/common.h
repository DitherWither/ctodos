#ifndef _COMMON_H_
#define _COMMON_H_

#include <config.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define dbg_print() fprintf(stderr, "%s: %d\n", __func__, __LINE__)

/// Print error message and quit
void fatal_error(const char *fmt, ...);

char *str_replace(char *orig, char *rep, char *with);

char from_hex(char ch);
char to_hex(char code);
char *url_encode(char *str);
char *url_decode(char *str);
#endif

#ifndef _ROUTER_H_
#define _ROUTER_H_

#include "common.h"
#include "connection.h"

struct ParsedRequest {
	char path[HTTP_PATH_MAX_SIZE];
	char method[8];
	char http_version[16];
	char *headers[HTTP_MAX_HEADERS];
	char *body;
};

void router_handle_request(struct Request *);
struct ParsedRequest *parse_request(struct Request *raw_req);
void print_body(char *buffer, char *body);

#endif

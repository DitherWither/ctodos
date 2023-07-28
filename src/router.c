#include "common.h"
#include "router.h"
#include "connection.h"
#include "todos.h"
#include <stdio.h>

char template[HTTP_MAX_BODY_SIZE / 2];

void index_handler(struct ParsedRequest *req, char *res);
void not_found_handler(struct ParsedRequest *req, char *res);
char *str_replace(char *orig, char *rep, char *with);
char *load_file(char const *path);

void load_template()
{
	FILE *fp = fopen("template.html", "r");
	char c;
	if (fp != NULL) {
		size_t newLen = fread(template, sizeof(char), HTTP_MAX_BODY_SIZE / 2, fp);
		if (ferror(fp) != 0) {
			fputs("Error reading file", stderr);
		} else {
			template[newLen++] = '\0'; /* Just to be safe. */
		}

		fclose(fp);
	} else {
		fatal_error("Could not open file");
	}
	strcat(template, "\0");
}

void router_handle_request(struct Request *raw_req)
{
	struct ParsedRequest *req = parse_request(raw_req);

	char *buffer = malloc(HTTP_RESPONSE_MAX_SIZE);
	memset(buffer, 0, HTTP_RESPONSE_MAX_SIZE);

	if (strcmp("/", req->path) == 0) {
		index_handler(req, buffer);
		goto end_routing;
	}

	not_found_handler(req, buffer);

end_routing:
	write(raw_req->connfd, buffer, strlen(buffer));

	free(buffer);
	free(req);
}

void not_found_handler(struct ParsedRequest *req, char *res)
{
	sprintf(res, "HTTP/1.1 200 OK\r\n");
	sprintf(res + strlen(res), "Content-Type:text/html\r\n");

	char body[HTTP_MAX_BODY_SIZE];

	sprintf(body, "<h1>Not Found</h1>");

	print_body(res, body);
}

void index_handler(struct ParsedRequest *req, char *res)
{
	sprintf(res, "HTTP/1.1 200 OK\r\n");
	sprintf(res + strlen(res), "Content-Type:text/html\r\n");

	char *body_inner = malloc(HTTP_MAX_BODY_SIZE / 2);

	sprintf(body_inner, "%s", "<h1>Todos List</h1><ul>");

	struct TodoItem *todo_cursor = todos_get_head();

	while (todo_cursor != NULL) {
		sprintf(body_inner + strlen(body_inner),
			"<li>%s: ", todo_cursor->title);
		switch (todo_cursor->type) {
		case TODOS_TYPE_COMPLETE:
			sprintf(body_inner + strlen(body_inner),
				"Complete</li>");
			break;
		case TODOS_TYPE_IN_PROGRESS:
			sprintf(body_inner + strlen(body_inner),
				"In Progress</li>");
			break;
		case TODOS_TYPE_INCOMPLETE:
			sprintf(body_inner + strlen(body_inner),
				"Incomplete</li>");
			break;
		default:
			sprintf(body_inner + strlen(body_inner),
				"Unknown</li>");
			break;
		}
		todo_cursor = todo_cursor->next;
	}
	sprintf(body_inner + strlen(body_inner), "</ul>");
	char *body = malloc(HTTP_MAX_BODY_SIZE);
	strcpy(body, template);
	body = str_replace(body, "{{slot}}", body_inner);

	print_body(res, body);
	free(body);
	free(body_inner);
}

struct ParsedRequest *parse_request(struct Request *raw_req)
{
	struct ParsedRequest *req = malloc(sizeof(struct ParsedRequest));

	char *headers_start = 0;

	sscanf(raw_req->buffer, "%8s %256s %16s", req->method, req->path,
	       req->http_version);

	// This code is utter garbage lol
	for (int i = 4; i < strlen(raw_req->buffer); i++) {
		if (raw_req->buffer[i - 1] == '\n' &&
		    raw_req->buffer[i - 2] == '\r') {
			if (raw_req->buffer[i - 3] == '\n' &&
			    raw_req->buffer[i - 4] == '\r') {
				// If is end of header
				req->body = &raw_req->buffer[i];
				break;
			}
			if (headers_start == 0) {
				headers_start = &raw_req->buffer[i];
			}
		}
	}

	char *token;
	int i = 0;
	while ((token = strsep(&headers_start, "\r\n"))) {
		req->headers[i] = token;
		i++;
	}

	return req;
}

void print_body(char *buffer, char *body)
{
	int len = strlen(body);

	sprintf(buffer + strlen(buffer), "Content-Length:%d\r\n", len);
	sprintf(buffer + strlen(buffer), "\r\n");
	sprintf(buffer + strlen(buffer), "%s", body);
}

// This code was copied from SO, as I can't be bothered
// with this lol
char *str_replace(char *orig, char *rep, char *with)
{
	char *result; // the return string
	char *ins; // the next insert point
	char *tmp; // varies
	int len_rep; // length of rep (the string to remove)
	int len_with; // length of with (the string to replace rep with)
	int len_front; // distance between rep and end of last rep
	int count; // number of replacements

	// sanity checks and initialization
	if (!orig || !rep)
		return NULL;
	len_rep = strlen(rep);
	if (len_rep == 0)
		return NULL; // empty rep causes infinite loop during count
	if (!with)
		with = "";
	len_with = strlen(with);

	// count the number of replacements needed
	ins = orig;
	for (count = 0; (tmp = strstr(ins, rep)); ++count) {
		ins = tmp + len_rep;
	}

	tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

	if (!result)
		return NULL;

	// first time through the loop, all the variable are set correctly
	// from here on,
	//    tmp points to the end of the result string
	//    ins points to the next occurrence of rep in orig
	//    orig points to the remainder of orig after "end of rep"
	while (count--) {
		ins = strstr(orig, rep);
		len_front = ins - orig;
		tmp = strncpy(tmp, orig, len_front) + len_front;
		tmp = strcpy(tmp, with) + len_with;
		orig += len_front + len_rep; // move to next "end of rep"
	}
	strcpy(tmp, orig);
	return result;
}

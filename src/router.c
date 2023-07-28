#include "common.h"
#include "router.h"
#include "connection.h"
#include "todos.h"
#include <stdio.h>

char template[HTTP_MAX_BODY_SIZE / 2];

void index_handler(struct ParsedRequest *req, char *res);
void not_found_handler(struct ParsedRequest *req, char *res);
char *str_replace(char *orig, char *rep, char *with);

/// Load the entire template.html file into memory
void load_template()
{
	FILE *fp = fopen("template.html", "r");
	char c;
	if (fp != NULL) {
		size_t newLen = fread(template, sizeof(char),
				      HTTP_MAX_BODY_SIZE / 2, fp);
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

	// Buffer for the body
	char *buffer = malloc(HTTP_RESPONSE_MAX_SIZE);
	memset(buffer, 0, HTTP_RESPONSE_MAX_SIZE);

	if (strcmp("/", req->path) == 0) {
		index_handler(req, buffer);
		// Edsger Dijkstra will haunt my soul in the afterlife for this
		goto end_routing;
	}

	// If nothing called goto yet, assume that nothing
	// handles it, and display 404 not found
	not_found_handler(req, buffer);

end_routing:
	write(raw_req->connfd, buffer, strlen(buffer));

	free(buffer);
	free(req);
}

/// Handler for the 404 not found page
void not_found_handler(struct ParsedRequest *req, char *res)
{
	sprintf(res, "HTTP/1.1 404 Not Found\r\n");
	sprintf(res + strlen(res), "Content-Type:text/html\r\n");

	char body[HTTP_MAX_BODY_SIZE];

	sprintf(body, "<h1>Not Found</h1>");

	print_body(res, body);
}

/// handler for the `/` route
void index_handler(struct ParsedRequest *req, char *res)
{
	if (strcmp(req->method, "POST") == 0) {
		// Copy body to a buffer
		char *title = malloc(32);
                memset(title, 0, 32);
		char status[16];
                memset(status, 0, 16);

		// Repeatedly call str_split to get individual fields
		// as an array
		// Allocating 2 extra fields just in case
		char *fields[4] = { NULL, NULL, NULL, NULL };

		char *token;
		int i = 0;
		while ((token = strsep(&req->body, "&"))) {
			fields[i] = token;
			i++;
		}

                for (i = 0; i < 4; i++) {
                        if (fields[i] == NULL) break;
                        char* name = strsep(&fields[i], "=");
                        if (strcmp("title", name) == 0) {
                                strcpy(title, fields[i]);
                        }
                        if (strcmp("status", name) == 0) {
                                strcpy(status, fields[i]);
                        }

                }

		printf("%s: %s\n", title, status);

		fflush(stdout);
	}
	sprintf(res, "HTTP/1.1 200 OK\r\n");
	sprintf(res + strlen(res), "Content-Type:text/html\r\n");

	// Inner body we write stuff to
	// the outer body will wrap this with the contents of
	// template.html
	char *body_inner = malloc(HTTP_MAX_BODY_SIZE / 2);
	memset(body_inner, 0, HTTP_MAX_BODY_SIZE / 2);

	// todos are stored in a linked list.
	struct TodoItem *todo_cursor = todos_get_head();

	// This writes the whole todos list into the
	// unordered list

	sprintf(body_inner, "<ul>");
	while (todo_cursor != NULL) {
		sprintf(body_inner + strlen(body_inner),
			"<li>%s: ", todo_cursor->title);

		// Convert the todos type to string
		// TODO: Move this to a function in todos.h
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
	// Copy template so that we don't accidentally mutate it
	strcpy(body, template);
	body = str_replace(body, "{{slot}}", body_inner);

	print_body(res, body);
	free(body);
	free(body_inner);
}

/// Takes a raw, unparsed request, and returns
/// a parsed request.
///
/// It internally mutates the raw request, and
/// doesn't copy the memory of the request
/// TODO: fix this, and copy the memory
struct ParsedRequest *parse_request(struct Request *raw_req)
{
	struct ParsedRequest *req = malloc(sizeof(struct ParsedRequest));

	char *headers_start = 0;

	// Get the data from first line.
	sscanf(raw_req->buffer, "%8s %256s %16s", req->method, req->path,
	       req->http_version);

	// This code is utter garbage lol
	//
	// We are starting at 4, as we want to be able to look
	// behind atleast 4 characters to look for "\r\n\r\n"
	// and the first line is guaranteed to be longer than
	// 4 characters anyways.
	for (int i = 4; i < strlen(raw_req->buffer); i++) {
		if (raw_req->buffer[i - 1] == '\n' &&
		    raw_req->buffer[i - 2] == '\r') {
			// If the cursor is at start of a newline
			if (raw_req->buffer[i - 3] == '\n' &&
			    raw_req->buffer[i - 4] == '\r') {
				// If is at the end of header
				//
				// The headers section ends with \r\n\r\n
				// aka empty line.
				req->body = &raw_req->buffer[i];
				break;
			}
			if (headers_start == 0) {
				// If this is after the end of first line,
				// that means that we are at the start of
				// the headers section
				headers_start = &raw_req->buffer[i];
			}
		}
	}

	// Split the headers into the headers array
	// TODO: split each header into key-value pairs
	char *token;
	int i = 0;
	while ((token = strsep(&headers_start, "\r\n"))) {
		req->headers[i] = token;
		i++;
	}

	return req;
}

/// Print the body into the request
void print_body(char *buffer, char *body)
{
	int len = strlen(body);

	sprintf(buffer + strlen(buffer), "Content-Length:%d\r\n", len);
	sprintf(buffer + strlen(buffer), "\r\n");
	sprintf(buffer + strlen(buffer), "%s", body);
}

/// This code was copied from SO, as I can't be bothered
/// with this lol
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

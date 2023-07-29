#include <config.h>
#include "todos.h"
#include "common.h"
#include "connection.h"
#include "router.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

struct Connection conn;

void quit()
{
        while (todos_get_head() != NULL) {
                todos_remove_by_index(0);
        }
        shutdown(conn.listenfd, SHUT_RD);
        close(conn.listenfd);
        exit(1);
}


int main(int argc, char **argv)
{
	conn = connection_new(SERVER_PORT);

	struct TodoItem *test_item = malloc(sizeof(struct TodoItem));
	sprintf(test_item->title, "Add a way to create todo items");
	test_item->type = TODOS_TYPE_INCOMPLETE;
	todos_insert_element(test_item);

	struct TodoItem *test_item2 = malloc(sizeof(struct TodoItem));
	sprintf(test_item2->title, "Create a display for the todos");
	test_item2->type = TODOS_TYPE_COMPLETE;
	todos_insert_element(test_item2);

	struct TodoItem test_item3;

	load_template();

        signal(SIGINT, quit);

	while (true) {
		struct Request *req = connection_get_request(&conn);

		router_handle_request(req);

		connection_delete_request(req);
	}

        quit();
}

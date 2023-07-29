#include "todos.h"
#include "common.h"

struct TodoItem *todos_head = NULL;
struct TodoItem *todos_tail = NULL;

struct TodoItem *todos_get_head()
{
	return todos_head;
}

struct TodoItem *todos_get_tail()
{
	return todos_tail;
}

struct TodoItem *todos_get_by_index(int idx)
{
	struct TodoItem *cursor = todos_head;

	if (cursor == NULL) {
		return NULL;
	}

	for (int i = 0; i < idx; i++) {
		if (cursor->next != NULL)
			cursor = cursor->next;
	}

	return cursor;
}

struct TodoItem *todos_get_from_end_by_index(int idx)
{
	struct TodoItem *cursor = todos_head;

	if (cursor == NULL) {
		return NULL;
	}

	for (int i = 0; i < idx; i++) {
		if (cursor->next != NULL)
			cursor = cursor->previous;
	}

	return cursor;
}

void todos_insert_element(struct TodoItem *item)
{
	if (todos_tail == NULL || todos_head == NULL) {
		todos_head = item;
		todos_tail = item;
		todos_head->next = NULL;
		todos_tail->previous = NULL;
                return;
	}

	item->next = NULL;
	item->previous = todos_tail;
	todos_tail->next = item;
	todos_tail = todos_tail->next;
}

int todos_get_length()
{
	struct TodoItem *cursor = todos_head;
	int len = 0;
	while (cursor != NULL) {
		cursor = cursor->next;
		len++;
	}
	return len;
}

char *todos_type_to_string(struct TodoItem *item)
{
	char *type;
	switch (item->type) {
	case TODOS_TYPE_COMPLETE:
		type = "Complete";
		break;
	case TODOS_TYPE_IN_PROGRESS:
		type = "In Progress";
		break;
	case TODOS_TYPE_INCOMPLETE:
		type = "Incomplete";
		break;
	default:
		type = "Unknown";
		break;
	}

	return type;
}

int todos_type_from_string(char *str)
{
	if (strcmp(str, "complete") == 0) {
		return TODOS_TYPE_COMPLETE;
	}

	if (strcmp(str, "incomplete") == 0) {
		return TODOS_TYPE_INCOMPLETE;
	}

	if (strcmp(str, "in-progress") == 0) {
		return TODOS_TYPE_IN_PROGRESS;
	}

	// If the type wasn't known
	return -1;
}

void todos_remove_by_index(int idx)
{
	struct TodoItem *todo_item = todos_get_by_index(idx);

        // this odd structure is so that if head == tail, it will still handle it properly
        if (todo_item == todos_head || todo_item == todos_tail) {
                if (todo_item == todos_head) {
                        todos_head = todo_item->next;
                }

                if (todo_item == todos_tail) {
                        todos_tail = todo_item->previous;
                }
                dbg_print();
                free(todo_item);

                return;
        }
        struct TodoItem* tmp = todo_item;

	if (todo_item->previous != NULL && todo_item->previous->next != NULL) {
		todo_item->previous->next = todo_item->next;
        }
	if (todo_item->next != NULL && todo_item->next->previous != NULL) {
		todo_item->next->previous = todo_item->previous;
        }
        dbg_print();
        free(tmp);
}